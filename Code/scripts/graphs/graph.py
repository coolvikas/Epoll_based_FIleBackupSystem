import matplotlib.pyplot as plt
import pandas as pd
fields = ['Threads', 'Th','Rt']
df = pd.read_csv('exp_results/finalresult.csv')
print(df)
x=df['Threads']

y=df['Rt']

plt.plot(x,y,label='Response Time')
plt.title('Response Time Analysis') 
plt.xlabel('No of Threads')
plt.ylabel('Response Time(in seconds)')
plt.legend()
plt.savefig('rt.png')
plt.clf()

