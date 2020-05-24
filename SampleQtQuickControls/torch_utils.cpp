#include "torch_utils.h"
#undef slots
#include "torch/script.h"
#define slots Q_SLOTS
#include <memory>
#include <iostream>
#include <QDebug>

torchUtils::torchUtils(QObject *parent)
    : QObject(parent)
{

}

QString torchUtils::load_model()
{
    QString msg;
    torch::jit::script::Module module;

    try {
      // Deserialize the ScriptModule from a file using torch::jit::load().
      module = torch::jit::load("../traced_resnet_model.pt");
    }
    catch (const c10::Error& e) {
      msg.append("Error loading model. Please check if the model is in the right directory\n");
      return msg;
    }
    msg.append("Model successfully loaded.\nPlease enter the path of the image to be classified\n");
    return msg;
}


QString torchUtils::predict_output(QString img_path)
{
    QString output;
    // Using openCV for preprocessing the input image

    //    // Create a vector of inputs.
    //    std::vector<torch::jit::IValue> inputs;
    //    inputs.push_back(torch::ones({1, 3, 224, 224}));

    //    // Execute the model and turn its output into a tensor.
    //    at::Tensor output = module.forward(inputs).toTensor();
    //    std::cout << output.slice(/*dim=*/1, /*start=*/0, /*end=*/5) << '\n';

    output.append("prediction successful");
    return output;
}
