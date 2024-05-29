import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# Load the CSV file into a DataFrame
df = pd.read_csv('/home/erikck/perf-oriented-dev/sol/9/results.csv')

# Ensure the data types are correct
df = df[df['Operations Completed'].notna()]
df['Operations Completed'] = df['Operations Completed'].astype(int)

# Plot the data
# Group by 'data structure' and sum 'Operations Completed'
grouped_df = df.groupby('data structure')['Operations Completed'].sum().reset_index()

# Create a bar plot
plt.figure(figsize=(10, 6))
sns.set(style="whitegrid")
plot = sns.barplot(x='data structure', y='Operations Completed', data=grouped_df)

# Set plot labels and title
plot.set_title('Total Operations by Data Structure')
plot.set_xlabel('Data Structure')
plot.set_ylabel('Total Operations Completed')

# Save plot
plt.savefig('/home/erikck/perf-oriented-dev/sol/9/total_operations_by_data_structure.png')
