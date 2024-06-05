import matplotlib.pyplot as plt 
import pandas as pd 
file = pd.read_excel("/ADC Calibration/data.xlsx") 
x_axis = file['DAC Value'] 
y_axis = file['Voltage'] 
plt.scatter(x_axis, y_axis, 2, 'red')
plt.title('Response of ESP32-S DAC')
plt.xlabel('DAC Value')
plt.ylabel('Voltage')
plt.show() 