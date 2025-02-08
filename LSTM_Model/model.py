import matplotlib.pyplot as plt
import pandas as pd
import torch
import torch.nn as nn
import torch.optim as optim
import torch.utils.data as data
import numpy as np
import math

import string
import re


opt = ""
findOutgoingLine = False


class nnModel(nn.Module):
    def __init__(self):
        super().__init__()
        self.lstm = nn.LSTM(input_size=1, hidden_size=50, num_layers=1, batch_first=True)
        self.linear = nn.Linear(50, 1)
    def forward(self, x):
        x, _ = self.lstm(x)
        x = self.linear(x)
        return x
    

def transformD(value,lookback):
    temp = pd.DataFrame({'AvgRate': [value, value, value, value]}, index=['0', '1', '2', '3'])
    dataset = temp[["AvgRate"]].values.astype('float32')
    
    X, y = [], []
    for i in range(len(dataset)-lookback):
        feature = dataset[i:i+lookback]
        target = dataset[i+1:i+lookback+1]
        X.append(feature)
        y.append(target)

    return torch.tensor(np.array(X)), torch.tensor(np.array(y))
def get_prediction(x):
    predictions=model(x)
    return predictions


model = nnModel()
model.load_state_dict(torch.load("trained_model.pth", weights_only=True))
model.eval()


if __name__ == "__main__":

    with open("nload.log",'w') as file:
                pass

    try:
        while True:
            with open("nload.log", "rb") as file:
                data = file.read()
            with open("nload.log",'w') as file:
                pass
            for byte in data:
                if chr(byte) in string.printable:
                    opt = opt + chr(byte) + ""
                else:
                    opt = opt + "\n"

            for line in opt.splitlines():
                if "Incoming" in line:
                    findOutgoingLine = False
                if "Outgoing" in line:
                    findOutgoingLine = True
                if "Curr" in line and findOutgoingLine:
                    arr = re.findall(r"Curr: (\d+\.\d+) (.+/s)",line)
                    if len(arr)>0:
                        val, scale = arr[0]
                        # print(val, "", scale)
                        if scale == "kBit/s" or scale == "Bit/s":
                            print("Failure!")
                        elif scale == "MBit/s":
                            X,Y = transformD(val, 3)
                            prediction,real = get_prediction(X),Y
                            numpy_array_prediction,numpy_array_real = prediction.detach().numpy(),real.detach().numpy()
                            print("Current calculated BW:", val, " MBit/s" ," | Predicted BW:", np.min(numpy_array_prediction) -1, " MBit/s", " | Adjsuted monitoring inteval: ", math.floor(1500 / ((np.min(numpy_array_prediction) -1) / 8)) , "us")
                        
                    findOutgoingLine = False

            opt = ""
    except KeyboardInterrupt:
        print('interrupted!')

