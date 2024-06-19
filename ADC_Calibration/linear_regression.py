import matplotlib.pyplot as plt 
import pandas as pd 
from scipy import stats
file = pd.read_excel("ADC_Calibration\data.xlsx") 
x_axis = file['ADC Sample Mean'] 
y_axis = file['Voltage'] 
plt.scatter(x_axis, y_axis, 2, 'red')
plt.title('Response of ESP32-S ADC')
plt.xlabel('ADC Sample Mean')
plt.ylabel('Voltage')

filtered_data_x = x_axis[10:37]
filtered_data_y = y_axis[10:37]
slope, intercept, r, p, std_err = stats.linregress(filtered_data_x, filtered_data_y)
def linear_regression(x):
  return slope * x + intercept
print(f"Linear Regression Function y = {slope}* x + {intercept}")
regression_model = list(map(linear_regression, filtered_data_x))
plt.plot(filtered_data_x, regression_model, 2, "blue")
plt.show();