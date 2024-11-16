// card_game_ai.h
#pragma once

#include <random>
#include <vector>
#include <string>
#include <deque>
#include "game_core.h"
#include "tensorflow/c/c_api.h"



constexpr int ACTION_SIZE = 36;  
constexpr int STATE_SIZE = 44;


struct TrainingExample {
    CompactGameState state;
    std::vector<float> target;
    int action;
    float reward;
    CompactGameState next_state;
    bool done;


    TrainingExample(const CompactGameState& s, int a, float r, const CompactGameState& ns, bool d)
        : state(s)
        , action(a)
        , reward(r)
        , next_state(ns)
        , done(d) {
        target.resize(ACTION_SIZE, 0.0f);
    }

 
    TrainingExample(const CompactGameState& s, const std::vector<float>& t, float r)
        : state(s)
        , target(t)
        , action(-1) 
        , reward(r)
        , next_state()
        , done(false) {}
};

class CardGameAI {
public:
    static const int STATE_SIZE = 44;
    static const int ACTION_SIZE = 36;
    static const int BATCH_SIZE = 32;
    static const int MEMORY_SIZE = 10000;
    static void createNetwork(TF_Graph* graph, TF_Operation** input_op, TF_Operation** output_op);
    CardGameAI();
    ~CardGameAI();
    static void initializeVariables(TF_Graph* graph, TF_Session* session);
    int getAction(const CompactGameState& state, float epsilon);
    void collectTrainingData(int numGames);
    void train(const std::vector<TrainingExample>& trainingData);
    std::vector<float> predict(const CompactGameState& state, bool use_target_network = false);
    void updateTargetNetwork();
    std::vector<float> gameStateToVector(const CompactGameState& state);

private:
    void initializeNetworks();
    void checkStatus(TF_Status* status);
    void saveCheckpoint(const std::string& filename);
    TF_Tensor* createStringTensor(const std::string& str);
    std::vector<int> sampleBatch();
    void storeExperience(const CompactGameState& state, 
                        int action, 
                        float reward,
                        const CompactGameState& next_state, 
                        bool done);
    TF_Tensor* vectorToTensor(const std::vector<float>& vec);
    std::vector<float> tensorToVector(TF_Tensor* tensor);


    int training_steps_;
    float best_loss_;
    std::random_device rd_;
    std::mt19937 gen_;
    std::uniform_real_distribution<> dis_;

    TF_Graph* main_graph_;
    TF_Graph* target_graph_;
    TF_Session* main_session_;
    TF_Session* target_session_;
    TF_Operation* main_input_op_;
    TF_Operation* main_output_op_;
    TF_Operation* target_input_op_;
    TF_Operation* target_output_op_;

    std::deque<TrainingExample> replay_buffer_;
};
