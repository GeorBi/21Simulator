// card_game_ai.cpp
#include "card_game_ai.h"
#include <iostream>
#include <cstring>
#include <tensorflow/c/c_api.h>
#include <vector>
#include <cmath>
#include <stdexcept>

CardGameAI::CardGameAI() 
    : training_steps_(0)
    , best_loss_(std::numeric_limits<float>::max())
    , rd_()
    , gen_(rd_())
    , dis_(0.0, 1.0)
    , main_graph_(TF_NewGraph())
    , target_graph_(TF_NewGraph())
    , main_session_(nullptr)
    , target_session_(nullptr)
    , main_input_op_(nullptr)
    , main_output_op_(nullptr)
    , target_input_op_(nullptr)
    , target_output_op_(nullptr) {
    
    main_graph_ = TF_NewGraph();
    target_graph_ = TF_NewGraph();
    
    TF_Status* status = TF_NewStatus();
    
    createNetwork(main_graph_, &main_input_op_, &main_output_op_);
    createNetwork(target_graph_, &target_input_op_, &target_output_op_);
    

    TF_SessionOptions* opts = TF_NewSessionOptions();
    main_session_ = TF_NewSession(main_graph_, opts, status);
    checkStatus(status);
    
    target_session_ = TF_NewSession(target_graph_, opts, status);
    checkStatus(status);
    
    TF_DeleteSessionOptions(opts);
    TF_DeleteStatus(status);
    
    initializeNetworks();
}

CardGameAI::~CardGameAI() {
    TF_Status* status = TF_NewStatus();
    
    TF_CloseSession(main_session_, status);
    TF_DeleteSession(main_session_, status);
    TF_DeleteGraph(main_graph_);
    
    TF_CloseSession(target_session_, status);
    TF_DeleteSession(target_session_, status);
    TF_DeleteGraph(target_graph_);
    
    TF_DeleteStatus(status);
}

void CardGameAI::createNetwork(TF_Graph* graph, TF_Operation** input_op, TF_Operation** output_op) {
    TF_Status* status = TF_NewStatus();
    
   
    TF_OperationDescription* input_desc = TF_NewOperation(graph, "Placeholder", "input");
    TF_SetAttrType(input_desc, "dtype", TF_FLOAT);
    std::vector<int64_t> input_dims = {-1, STATE_SIZE};
    TF_SetAttrShape(input_desc, "shape", input_dims.data(), input_dims.size());
    *input_op = TF_FinishOperation(input_desc, status);
    
 
    TF_OperationDescription* w1_init_desc = TF_NewOperation(graph, "Const", "w1/initial_value");
    TF_SetAttrType(w1_init_desc, "dtype", TF_FLOAT);
    std::vector<int64_t> w1_dims = {STATE_SIZE, 64};
    std::vector<float> w1_init_data(STATE_SIZE * 64);
    float w1_range = sqrt(6.0f / (STATE_SIZE + 64));
    for (auto& w : w1_init_data) {
        w = (((float)rand() / RAND_MAX) * 2 - 1) * w1_range;
    }
    TF_Tensor* w1_init_tensor = TF_AllocateTensor(TF_FLOAT, w1_dims.data(), 2, sizeof(float) * w1_init_data.size());
    std::memcpy(TF_TensorData(w1_init_tensor), w1_init_data.data(), sizeof(float) * w1_init_data.size());
    TF_SetAttrTensor(w1_init_desc, "value", w1_init_tensor, status);
    TF_Operation* w1_init = TF_FinishOperation(w1_init_desc, status);

    TF_OperationDescription* w1_desc = TF_NewOperation(graph, "Variable", "w1");
    TF_SetAttrType(w1_desc, "dtype", TF_FLOAT);
    TF_SetAttrShape(w1_desc, "shape", w1_dims.data(), w1_dims.size());
    TF_Operation* w1 = TF_FinishOperation(w1_desc, status);

 
    TF_OperationDescription* b1_init_desc = TF_NewOperation(graph, "Const", "b1/initial_value");
    TF_SetAttrType(b1_init_desc, "dtype", TF_FLOAT);
    std::vector<int64_t> b1_dims = {64};
    std::vector<float> b1_init_data(64, 0.0f);
    TF_Tensor* b1_init_tensor = TF_AllocateTensor(TF_FLOAT, b1_dims.data(), 1, sizeof(float) * b1_init_data.size());
    std::memcpy(TF_TensorData(b1_init_tensor), b1_init_data.data(), sizeof(float) * b1_init_data.size());
    TF_SetAttrTensor(b1_init_desc, "value", b1_init_tensor, status);
    TF_Operation* b1_init = TF_FinishOperation(b1_init_desc, status);

    TF_OperationDescription* b1_desc = TF_NewOperation(graph, "Variable", "b1");
    TF_SetAttrType(b1_desc, "dtype", TF_FLOAT);
    TF_SetAttrShape(b1_desc, "shape", b1_dims.data(), b1_dims.size());
    TF_Operation* b1 = TF_FinishOperation(b1_desc, status);

  
    TF_OperationDescription* matmul1_desc = TF_NewOperation(graph, "MatMul", "matmul1");
    TF_AddInput(matmul1_desc, {*input_op, 0});
    TF_AddInput(matmul1_desc, {w1, 0});
    TF_Operation* matmul1 = TF_FinishOperation(matmul1_desc, status);

   
    TF_OperationDescription* add1_desc = TF_NewOperation(graph, "Add", "add1");
    TF_AddInput(add1_desc, {matmul1, 0});
    TF_AddInput(add1_desc, {b1, 0});
    TF_Operation* add1 = TF_FinishOperation(add1_desc, status);


    TF_OperationDescription* relu1_desc = TF_NewOperation(graph, "Relu", "relu1");
    TF_AddInput(relu1_desc, {add1, 0});
    TF_Operation* relu1 = TF_FinishOperation(relu1_desc, status);

   
    TF_OperationDescription* w2_init_desc = TF_NewOperation(graph, "Const", "w2/initial_value");
    TF_SetAttrType(w2_init_desc, "dtype", TF_FLOAT);
    std::vector<int64_t> w2_dims = {64, ACTION_SIZE};
    std::vector<float> w2_init_data(64 * ACTION_SIZE);
    float w2_range = sqrt(6.0f / (64 + ACTION_SIZE));
    for (auto& w : w2_init_data) {
        w = (((float)rand() / RAND_MAX) * 2 - 1) * w2_range;
    }
    TF_Tensor* w2_init_tensor = TF_AllocateTensor(TF_FLOAT, w2_dims.data(), 2, sizeof(float) * w2_init_data.size());
    std::memcpy(TF_TensorData(w2_init_tensor), w2_init_data.data(), sizeof(float) * w2_init_data.size());
    TF_SetAttrTensor(w2_init_desc, "value", w2_init_tensor, status);
    TF_Operation* w2_init = TF_FinishOperation(w2_init_desc, status);

    TF_OperationDescription* w2_desc = TF_NewOperation(graph, "Variable", "w2");
    TF_SetAttrType(w2_desc, "dtype", TF_FLOAT);
    TF_SetAttrShape(w2_desc, "shape", w2_dims.data(), w2_dims.size());
    TF_Operation* w2 = TF_FinishOperation(w2_desc, status);


    TF_OperationDescription* b2_init_desc = TF_NewOperation(graph, "Const", "b2/initial_value");
    TF_SetAttrType(b2_init_desc, "dtype", TF_FLOAT);
    std::vector<int64_t> b2_dims = {ACTION_SIZE};
    std::vector<float> b2_init_data(ACTION_SIZE, 0.0f);
    TF_Tensor* b2_init_tensor = TF_AllocateTensor(TF_FLOAT, b2_dims.data(), 1, sizeof(float) * b2_init_data.size());
    std::memcpy(TF_TensorData(b2_init_tensor), b2_init_data.data(), sizeof(float) * b2_init_data.size());
    TF_SetAttrTensor(b2_init_desc, "value", b2_init_tensor, status);
    TF_Operation* b2_init = TF_FinishOperation(b2_init_desc, status);

    TF_OperationDescription* b2_desc = TF_NewOperation(graph, "Variable", "b2");
    TF_SetAttrType(b2_desc, "dtype", TF_FLOAT);
    TF_SetAttrShape(b2_desc, "shape", b2_dims.data(), b2_dims.size());
    TF_Operation* b2 = TF_FinishOperation(b2_desc, status);


    TF_OperationDescription* matmul2_desc = TF_NewOperation(graph, "MatMul", "matmul2");
    TF_AddInput(matmul2_desc, {relu1, 0});
    TF_AddInput(matmul2_desc, {w2, 0});
    TF_Operation* matmul2 = TF_FinishOperation(matmul2_desc, status);


    TF_OperationDescription* add2_desc = TF_NewOperation(graph, "Add", "output");
    TF_AddInput(add2_desc, {matmul2, 0});
    TF_AddInput(add2_desc, {b2, 0});
    *output_op = TF_FinishOperation(add2_desc, status);


    TF_OperationDescription* assign_w1_desc = TF_NewOperation(graph, "Assign", "assign_w1");
    TF_AddInput(assign_w1_desc, {w1, 0});
    TF_AddInput(assign_w1_desc, {w1_init, 0});
    TF_Operation* assign_w1 = TF_FinishOperation(assign_w1_desc, status);

    TF_OperationDescription* assign_b1_desc = TF_NewOperation(graph, "Assign", "assign_b1");
    TF_AddInput(assign_b1_desc, {b1, 0});
    TF_AddInput(assign_b1_desc, {b1_init, 0});
    TF_Operation* assign_b1 = TF_FinishOperation(assign_b1_desc, status);

    TF_OperationDescription* assign_w2_desc = TF_NewOperation(graph, "Assign", "assign_w2");
    TF_AddInput(assign_w2_desc, {w2, 0});
    TF_AddInput(assign_w2_desc, {w2_init, 0});
    TF_Operation* assign_w2 = TF_FinishOperation(assign_w2_desc, status);

    TF_OperationDescription* assign_b2_desc = TF_NewOperation(graph, "Assign", "assign_b2");
    TF_AddInput(assign_b2_desc, {b2, 0});
    TF_AddInput(assign_b2_desc, {b2_init, 0});
    TF_Operation* assign_b2 = TF_FinishOperation(assign_b2_desc, status);

    if (TF_GetCode(status) != TF_OK) {
        std::string error_msg = TF_Message(status);
        TF_DeleteStatus(status);
        throw std::runtime_error("Failed to create network: " + error_msg);
    }


    TF_DeleteTensor(w1_init_tensor);
    TF_DeleteTensor(b1_init_tensor);
    TF_DeleteTensor(w2_init_tensor);
    TF_DeleteTensor(b2_init_tensor);
    TF_DeleteStatus(status);
}

void CardGameAI::initializeVariables(TF_Graph* graph, TF_Session* session) {
    TF_Status* status = TF_NewStatus();


    {

        TF_Operation* w1 = TF_GraphOperationByName(graph, "w1");
        if (!w1) throw std::runtime_error("Failed to find w1 operation");
        
        std::vector<float> w1_data(STATE_SIZE * 64);
  
        float w1_range = sqrt(6.0f / (STATE_SIZE + 64));
        for (auto& w : w1_data) {
            w = (((float)rand() / RAND_MAX) * 2 - 1) * w1_range;
        }
        
        TF_Tensor* w1_tensor = TF_AllocateTensor(
            TF_FLOAT,
            std::vector<int64_t>{STATE_SIZE, 64}.data(),
            2,
            sizeof(float) * w1_data.size()
        );
        std::memcpy(TF_TensorData(w1_tensor), w1_data.data(), sizeof(float) * w1_data.size());
        
   
        TF_OperationDescription* assign_w1_desc = TF_NewOperation(graph, "Assign", "assign_w1");
        TF_Output w1_output = {w1, 0};
        TF_AddInput(assign_w1_desc, w1_output);
        TF_Operation* w1_init = TF_GraphOperationByName(graph, "w1/initial_value");
        TF_Output w1_init_output = {w1_init, 0};
        TF_AddInput(assign_w1_desc, w1_init_output);
        TF_Operation* assign_w1 = TF_FinishOperation(assign_w1_desc, status);
    }


    {
        TF_Operation* b1 = TF_GraphOperationByName(graph, "b1");
        if (!b1) throw std::runtime_error("Failed to find b1 operation");
        
        std::vector<float> b1_data(64, 0.0f);
        
        TF_Tensor* b1_tensor = TF_AllocateTensor(
            TF_FLOAT,
            std::vector<int64_t>{64}.data(),
            1,
            sizeof(float) * b1_data.size()
        );
        std::memcpy(TF_TensorData(b1_tensor), b1_data.data(), sizeof(float) * b1_data.size());
        
        TF_OperationDescription* assign_b1_desc = TF_NewOperation(graph, "Assign", "assign_b1");
        TF_Output b1_output = {b1, 0};
        TF_AddInput(assign_b1_desc, b1_output);
        TF_Operation* b1_init = TF_GraphOperationByName(graph, "b1/initial_value");
        TF_Output b1_init_output = {b1_init, 0};
        TF_AddInput(assign_b1_desc, b1_init_output);
        TF_Operation* assign_b1 = TF_FinishOperation(assign_b1_desc, status);
    }


    {
        TF_Operation* w2 = TF_GraphOperationByName(graph, "w2");
        if (!w2) throw std::runtime_error("Failed to find w2 operation");
        
        std::vector<float> w2_data(64 * ACTION_SIZE);
        float w2_range = sqrt(6.0f / (64 + ACTION_SIZE));
        for (auto& w : w2_data) {
            w = (((float)rand() / RAND_MAX) * 2 - 1) * w2_range;
        }
        
        TF_Tensor* w2_tensor = TF_AllocateTensor(
            TF_FLOAT,
            std::vector<int64_t>{64, ACTION_SIZE}.data(),
            2,
            sizeof(float) * w2_data.size()
        );
        std::memcpy(TF_TensorData(w2_tensor), w2_data.data(), sizeof(float) * w2_data.size());
        
        TF_OperationDescription* assign_w2_desc = TF_NewOperation(graph, "Assign", "assign_w2");
        TF_Output w2_output = {w2, 0};
        TF_AddInput(assign_w2_desc, w2_output);
        TF_Operation* w2_init = TF_GraphOperationByName(graph, "w2/initial_value");
        TF_Output w2_init_output = {w2_init, 0};
        TF_AddInput(assign_w2_desc, w2_init_output);
        TF_Operation* assign_w2 = TF_FinishOperation(assign_w2_desc, status);
    }


    {
        TF_Operation* b2 = TF_GraphOperationByName(graph, "b2");
        if (!b2) throw std::runtime_error("Failed to find b2 operation");
        
        std::vector<float> b2_data(ACTION_SIZE, 0.0f);  
        
        TF_Tensor* b2_tensor = TF_AllocateTensor(
            TF_FLOAT,
            std::vector<int64_t>{ACTION_SIZE}.data(),
            1,
            sizeof(float) * b2_data.size()
        );
        std::memcpy(TF_TensorData(b2_tensor), b2_data.data(), sizeof(float) * b2_data.size());
        
        TF_OperationDescription* assign_b2_desc = TF_NewOperation(graph, "Assign", "assign_b2");
        TF_Output b2_output = {b2, 0};
        TF_AddInput(assign_b2_desc, b2_output);
        TF_Operation* b2_init = TF_GraphOperationByName(graph, "b2/initial_value");
        TF_Output b2_init_output = {b2_init, 0};
        TF_AddInput(assign_b2_desc, b2_init_output);
        TF_Operation* assign_b2 = TF_FinishOperation(assign_b2_desc, status);
    }

    if (TF_GetCode(status) != TF_OK) {
        std::string error_msg = TF_Message(status);
        TF_DeleteStatus(status);
        throw std::runtime_error("Failed to initialize variables: " + error_msg);
    }

    TF_DeleteStatus(status);
}


void CardGameAI::initializeNetworks() {
    TF_Status* status = TF_NewStatus();
    

    {
        TF_OperationDescription* init_desc = 
            TF_NewOperation(main_graph_, "GlobalVariablesInitializer", "init");
        TF_Operation* init_op = TF_FinishOperation(init_desc, status);
        checkStatus(status);
        
        TF_SessionRun(
            main_session_,
            nullptr,
            nullptr, nullptr, 0,
            nullptr, nullptr, 0,
            &init_op, 1,
            nullptr,
            status
        );
        checkStatus(status);
    }
    

    {
        TF_OperationDescription* init_desc = 
            TF_NewOperation(target_graph_, "GlobalVariablesInitializer", "init");
        TF_Operation* init_op = TF_FinishOperation(init_desc, status);
        checkStatus(status);
        
        TF_SessionRun(
            target_session_,
            nullptr,
            nullptr, nullptr, 0,
            nullptr, nullptr, 0,
            &init_op, 1,
            nullptr,
            status
        );
        checkStatus(status);
    }
    
    TF_DeleteStatus(status);
}

int CardGameAI::getAction(const CompactGameState& state, float epsilon) {
    if (dis_(gen_) < epsilon) {

        return std::uniform_int_distribution<>(0, ACTION_SIZE-1)(gen_);
    }
    
 
    auto q_values = predict(state);
    return std::max_element(q_values.begin(), q_values.end()) - q_values.begin();
}

void CardGameAI::updateTargetNetwork() {
    TF_Status* status = TF_NewStatus();
    

    std::vector<TF_Operation*> trainable_vars;
    size_t pos = 0;
    TF_Operation* oper;
    
    while ((oper = TF_GraphNextOperation(main_graph_, &pos)) != nullptr) {
        const char* name = TF_OperationName(oper);
   
        if (strstr(name, "kernel") != nullptr || strstr(name, "bias") != nullptr) {
            trainable_vars.push_back(oper);
        }
    }
    

    for (auto var : trainable_vars) {
        const char* name = TF_OperationName(var);
        

        TF_Output main_output = {var, 0};
        TF_Tensor* value;
        TF_SessionRun(
            main_session_,
            nullptr,
            nullptr, nullptr, 0,
            &main_output, &value, 1,
            nullptr, 0,
            nullptr,
            status
        );
        checkStatus(status);
        

        TF_Operation* target_var = TF_GraphOperationByName(target_graph_, name);
        if (target_var) {
            TF_Output target_input = {target_var, 0};

            TF_SessionRun(
                target_session_,
                nullptr,
                &target_input, &value, 1,
                nullptr, nullptr, 0,
                nullptr, 0,
                nullptr,
                status
            );
            checkStatus(status);
        }
        
        TF_DeleteTensor(value);
    }
    
    TF_DeleteStatus(status);
}

void CardGameAI::collectTrainingData(int numGames) {
    std::vector<TrainingExample> trainingData;
    
    for (int i = 0; i < numGames; i++) {
        GameOperations game;
        std::vector<CompactGameState> gameStates;
        
        while (!game.isMatchOver()) {

            gameStates.push_back(game.getGameState());
            

            if (game.isPlayer1Turn()) {

                int action = getAction(game.getGameState(), 0.1f);
                
                if (action < 11) {
                    game.drawCard();
                } else {
                    game.useTrumpCard(action - 11);
                }
            } else {

                if (game.getGameState().getPlayer2Tally() < 17) {
                    game.drawCard();
                } else {
                    game.stand();
                }
            }
        }
        

        float reward = (game.getGameState().getWinState() == CompactGameState::WinState::PLAYER1_WIN) ? 1.0f : -1.0f;
        

        for (const auto& state : gameStates) {
            std::vector<float> target(ACTION_SIZE, 0.0f);
            trainingData.emplace_back(state, target, reward);
        }
    }
    

    train(trainingData);
}

std::vector<float> CardGameAI::gameStateToVector(const CompactGameState& state) {
    std::vector<float> stateVector;
    stateVector.reserve(44);


    uint16_t mainDeck = state.getMainDeck();
    for (int i = 0; i < 11; i++) {
        stateVector.push_back((mainDeck & (1 << i)) ? 1.0f : 0.0f);
    }

    uint16_t p1Deck = state.getPlayer1Deck();
    for (int i = 0; i < 11; i++) {
        stateVector.push_back((p1Deck & (1 << i)) ? 1.0f : 0.0f);
    }


    uint16_t p2Deck = state.getPlayer2Deck();
    for (int i = 0; i < 11; i++) {
        stateVector.push_back((p2Deck & (1 << i)) ? 1.0f : 0.0f);
    }


    stateVector.push_back(static_cast<float>(state.getPlayer1Tally()) / 31.0f);
    stateVector.push_back(static_cast<float>(state.getPlayer2Tally()) / 31.0f);
    stateVector.push_back(static_cast<float>(state.getGoal()) / 31.0f);
    stateVector.push_back(static_cast<float>(state.getPlayerDistance()) / 14.0f);
    stateVector.push_back(static_cast<float>(state.getCurrentRound()) / 10.0f);
    stateVector.push_back(static_cast<float>(state.getCurrentBet()) / 10.0f);
    

    stateVector.push_back(state.isPlayer1Turn() ? 1.0f : 0.0f);
    stateVector.push_back(state.isPlayer1Standing() ? 1.0f : 0.0f);
    stateVector.push_back(state.isPlayer2Standing() ? 1.0f : 0.0f);
    stateVector.push_back(state.isPlayer1Bust() ? 1.0f : 0.0f);
    stateVector.push_back(state.isPlayer2Bust() ? 1.0f : 0.0f);

    return stateVector;
}

void CardGameAI::train(const std::vector<TrainingExample>& trainingData) {
    if (trainingData.empty()) {
        std::cerr << "Error: Training data is empty" << std::endl;
        return;
    }

    const int BATCH_SIZE = 32;
    const int EPOCHS = 10;
    
    TF_Status* status = TF_NewStatus();
    
    try {

        TF_OperationDescription* target_desc = 
            TF_NewOperation(main_graph_, "Placeholder", "target_values");
        TF_SetAttrType(target_desc, "dtype", TF_FLOAT);
        int64_t target_dims[] = {-1, ACTION_SIZE};  
        TF_SetAttrShape(target_desc, "shape", target_dims, 2);
        TF_Operation* target_op = TF_FinishOperation(target_desc, status);
        checkStatus(status);


        TF_OperationDescription* diff_desc = 
            TF_NewOperation(main_graph_, "Sub", "difference");
        TF_AddInput(diff_desc, {main_output_op_, 0});
        TF_AddInput(diff_desc, {target_op, 0});
        TF_Operation* diff_op = TF_FinishOperation(diff_desc, status);
        checkStatus(status);

        TF_OperationDescription* square_desc = 
            TF_NewOperation(main_graph_, "Square", "squared_difference");
        TF_AddInput(square_desc, {diff_op, 0});
        TF_Operation* square_op = TF_FinishOperation(square_desc, status);
        checkStatus(status);

        TF_OperationDescription* reduce_mean_desc = 
            TF_NewOperation(main_graph_, "Mean", "loss");
        TF_AddInput(reduce_mean_desc, {square_op, 0});
        TF_Operation* loss_op = TF_FinishOperation(reduce_mean_desc, status);
        checkStatus(status);

 
        TF_OperationDescription* optimizer_desc = 
            TF_NewOperation(main_graph_, "AdamOptimizer", "optimizer");
        TF_SetAttrFloat(optimizer_desc, "learning_rate", 0.001f);
        TF_Operation* optimizer_op = TF_FinishOperation(optimizer_desc, status);
        checkStatus(status);


        TF_OperationDescription* train_desc = 
            TF_NewOperation(main_graph_, "MinimizeOp", "train");
        TF_AddInput(train_desc, {loss_op, 0});
        TF_AddInput(train_desc, {optimizer_op, 0});
        checkStatus(status);


        for (int epoch = 0; epoch < EPOCHS; epoch++) {
            float epoch_loss = 0.0f;
            int num_batches = (trainingData.size() + BATCH_SIZE - 1) / BATCH_SIZE;

            for (int batch = 0; batch < num_batches; batch++) {
            int batch_start = batch * BATCH_SIZE;
            int batch_end = std::min(batch_start + BATCH_SIZE, 
                                   static_cast<int>(trainingData.size()));
            int current_batch_size = batch_end - batch_start;

            std::vector<float> batch_states;
            std::vector<float> batch_targets;
            batch_states.reserve(current_batch_size * STATE_SIZE);
            batch_targets.reserve(current_batch_size * ACTION_SIZE);

            for (int i = batch_start; i < batch_end; i++) {
                const auto& example = trainingData[i];
     
                std::vector<float> state_vector = gameStateToVector(example.state);
                
       
                batch_states.insert(batch_states.end(), 
                                  state_vector.begin(), 
                                  state_vector.end());
                

                batch_targets.insert(batch_targets.end(), 
                                   example.target.begin(), 
                                   example.target.end());
            }
            }
        
            float avg_epoch_loss = epoch_loss / num_batches;
            std::cout << "Epoch " << (epoch + 1) << "/" << EPOCHS 
                     << " completed. Average Loss: " << avg_epoch_loss << std::endl;

 
            if (avg_epoch_loss < best_loss_) {
                best_loss_ = avg_epoch_loss;
                saveCheckpoint("best_model.ckpt");
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "Error during training: " << e.what() << std::endl;
    }

    TF_DeleteStatus(status);
}



void CardGameAI::saveCheckpoint(const std::string& filename) {
    TF_Status* status = TF_NewStatus();
    

    TF_OperationDescription* saver_desc = 
        TF_NewOperation(main_graph_, "Saver", "saver");
    TF_Operation* saver = TF_FinishOperation(saver_desc, status);
    checkStatus(status);
    

    TF_Tensor* filename_tensor = createStringTensor(filename);
    

    TF_Output save_input = {saver, 0};
    TF_SessionRun(
        main_session_,
        nullptr,
        &save_input, &filename_tensor, 1,
        nullptr, nullptr, 0,
        nullptr, 0,
        nullptr,
        status
    );
    checkStatus(status);
    
    TF_DeleteTensor(filename_tensor);
    TF_DeleteStatus(status);
}

TF_Tensor* CardGameAI::createStringTensor(const std::string& str) {
    size_t data_size = str.size() + 1; 
    TF_Tensor* tensor = TF_AllocateTensor(TF_STRING, nullptr, 0, data_size);
    memcpy(TF_TensorData(tensor), str.c_str(), data_size);
    return tensor;
}

std::vector<int> CardGameAI::sampleBatch() {
    std::vector<int> indices;
    indices.reserve(BATCH_SIZE);
    
    std::uniform_int_distribution<> dist(0, replay_buffer_.size() - 1);
    for (int i = 0; i < BATCH_SIZE; ++i) {
        indices.push_back(dist(gen_));
    }
    
    return indices;
}

void CardGameAI::storeExperience(const CompactGameState& state, 
                                int action, 
                                float reward, 
                                const CompactGameState& next_state, 
                                bool done) {
    if (replay_buffer_.size() >= MEMORY_SIZE) {
        replay_buffer_.pop_front();
    }
    
    replay_buffer_.push_back(TrainingExample(state, action, reward, next_state, done));
}

std::vector<float> CardGameAI::predict(const CompactGameState& state, bool use_target_network) {

    std::vector<float> stateVector = gameStateToVector(state);
    
    TF_Status* status = TF_NewStatus();
    

    TF_Tensor* input_tensor = vectorToTensor(stateVector);
    

    TF_Operation* input_op = use_target_network ? target_input_op_ : main_input_op_;
    TF_Operation* output_op = use_target_network ? target_output_op_ : main_output_op_;
    
    TF_Output input_port = {input_op, 0};
    TF_Output output_port = {output_op, 0};
    

    TF_Tensor* output_tensor;
    TF_SessionRun(
        use_target_network ? target_session_ : main_session_,
        nullptr,
        &input_port, &input_tensor, 1,
        &output_port, &output_tensor, 1,
        nullptr, 0,
        nullptr,
        status
    );
    checkStatus(status);
    

    std::vector<float> result = tensorToVector(output_tensor);
    

    TF_DeleteTensor(input_tensor);
    TF_DeleteTensor(output_tensor);
    TF_DeleteStatus(status);
    
    return result;
}

void CardGameAI::checkStatus(TF_Status* status) {
    if (TF_GetCode(status) != TF_OK) {
        std::string error_msg = TF_Message(status);
        throw std::runtime_error("TensorFlow Error: " + error_msg);
    }
}


TF_Tensor* CardGameAI::vectorToTensor(const std::vector<float>& vec) {
    const int64_t dims[] = {1, static_cast<int64_t>(vec.size())};
    const size_t data_size = vec.size() * sizeof(float);
    
    TF_Tensor* tensor = TF_AllocateTensor(TF_FLOAT, dims, 2, data_size);
    std::memcpy(TF_TensorData(tensor), vec.data(), data_size);
    
    return tensor;
}

std::vector<float> CardGameAI::tensorToVector(TF_Tensor* tensor) {
    float* data = static_cast<float*>(TF_TensorData(tensor));
    size_t size = TF_TensorByteSize(tensor) / sizeof(float);
    return std::vector<float>(data, data + size);
}
