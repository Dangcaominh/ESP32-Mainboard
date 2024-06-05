import matplotlib.pyplot as plt 
import pandas as pd 
file = pd.read_excel("data.xlsx") 
x_axis = file['ADC Sample Mean'] 
y_axis = file['Voltage'] 
for i in range(6, 49):
    z = (x_axis[i+1] - x_axis[i])/ (y_axis[i+1] - y_axis[i])
    print(f"Voltage = {y_axis[i]}, coeff = {z}")
plt.scatter(x_axis, y_axis, 2, 'red')
plt.title('Response of ESP32-S ADC')
plt.xlabel('ADC Sample Mean')
plt.ylabel('Voltage')
plt.show()
