## Download seismic data from the IGN (Spain) catalogue ##
## Modify the url string according to the Min Max Lat Lon, date, EQ magnitude, ...
import requests
import json
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import matplotlib.dates as mdates
# from numpy import random
from datetime import datetime as dt
import pandas as pd


# Download data from sensors
def download_data(url):
	try:
		r = requests.get(url)
		r.raise_for_status()
	except :
		print ('Sensor not reacheable')
		exit(0)
	return json.loads(r.text)

def retrieve_data():
  data_file = r'C:\PATH_to_FILE\sensor_data.txt'
	# Retrieve data
	x = dt.now().strftime('%d-%m-%Y\t%H:%M')
	t_out = float(download_data('http://192.168.1.46/things/Exterior/properties/').get('temperature'))
	t_in = float(download_data('http://192.168.1.45/things/Menjador/properties/').get('temperature'))
	# Check if data file exists and append data
	try:
		f = open(data_file,"a+") # r+ points at the begining and leave at the end to append new data
	except IOError:
		f= open(data_file,"a+")
		print("File not accessible")
	f.write("{}\t{}\t{}\n".format(x, t_in, t_out))
	f.close()
	text = pd.read_csv(data_file, header = None,  sep='\t')
	return text # Getting one column of the df text.iloc[:, 0]


def animate(i):
	data = retrieve_data()
	x = data.iloc[:,1].tolist()
	dx = [dt.strptime(i, '%H:%M') for i in x]
	y1 = data.iloc[:,2].tolist()
	y2 = data.iloc[:,3].tolist()
	plt.cla()
	ax = plt.gca()
	locator = mdates.AutoDateLocator(minticks=3, maxticks=7)
	formatter = mdates.DateFormatter("%H:%M")
	ax.xaxis.set_major_locator(locator)
	ax.xaxis.set_major_formatter(formatter)
	plt.plot(dx, y1, label = "T in", color = 'r')
	plt.plot(dx, y2, label = 'T out', color = 'b')
	plt.legend(loc= 'upper left')
	plt.xlabel('Time')
	plt.ylabel('Temperature (ºC)')
	plt.xticks(rotation=90)
	plt.tight_layout()


ani = FuncAnimation(plt.gcf(), animate, interval = 300000)
plt.show()
