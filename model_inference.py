import torch
import torch.nn as nn
import torch.nn.functional as F
from torchvision import transforms
from PIL import Image
import cv2
import numpy as np


class TinyVGGModel(nn.Module):
    def __init__(self):
        super(TinyVGGModel, self).__init__()
        # Convolutional layers
        self.conv1 = nn.Conv2d(3, 8, kernel_size=3, padding=1)
        self.conv2 = nn.Conv2d(8, 16, kernel_size=3, padding=1)
        self.conv3 = nn.Conv2d(16, 32, kernel_size=3, padding=1)
        # Max pooling layer
        self.pool = nn.MaxPool2d(2, 2)
        # Fully connected layers
        self.fc1 = nn.Linear(32 * 30 * 80, 128)  # Adjust dimensions as per input size
        self.fc2 = nn.Linear(128, 3)  # For 3 classes: Forward (0), Left (1), Right (2)
        self.dropout = nn.Dropout(0.5)

    def forward(self, x):
        # Apply convolutions and pooling
        x = self.pool(F.relu(self.conv1(x)))
        x = self.pool(F.relu(self.conv2(x)))
        x = self.pool(F.relu(self.conv3(x)))
        # Flatten for fully connected layers
        x = x.view(x.size(0), -1)
        x = F.relu(self.fc1(x))
        x = self.dropout(x)
        x = self.fc2(x)
        return x


def load_model(model_path, device):
    model = TinyVGGModel().to(device)
    model.load_state_dict(torch.load(model_path, map_location=device))
    model.eval()
    return model


def preprocess_frame(frame, transform, device):
    frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    pil_image = Image.fromarray(frame_rgb)
    input_image = transform(pil_image).unsqueeze(0).to(device)
    return input_image


def predict(model, input_image):
    try:
        with torch.no_grad():
            output = model(input_image)
            _, predicted_class = torch.max(output, 1)
            return predicted_class.item()
    except Exception as e:
        print(f"Error during prediction: {e}")
        return -1

