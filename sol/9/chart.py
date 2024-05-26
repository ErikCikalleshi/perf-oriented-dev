import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# Load the CSV file into a DataFrame
df = pd.read_csv('/home/erikck/perf-oriented-dev/sol/9/results.csv')

# Ensure the data types are correct
df['Operations Completed'] = df['Operations Completed'].astype(int)

# Plot the data
plt.figure(figsize=(14, 8))
sns.set(style="whitegrid")

# Create a point plot to compare operations completed
plot = sns.pointplot(x='Number of Elements', y='Operations Completed', hue='data structure', 
                     data=df, markers=["o", "s"], linestyles=["-", "--"])

# Set plot labels and title
plot.set_yscale("log")
plot.set_title('Operations Completed by Data Structure')
plot.set_xlabel('Number of Elements')
plot.set_ylabel('Operations Completed (log scale)')

# Rotate x-axis labels for better readability
plt.xticks(rotation=45)
plt.legend(title='Data Structure')

# Adjust the bottom margin to prevent labels from being cut off
plt.subplots_adjust(bottom=0.15)

# Save plot
plt.savefig('/home/erikck/perf-oriented-dev/sol/9/number_of_elements_vs_operations_completed2.png')
