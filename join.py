import sys

def main() :
	if len(sys.argv) != 2:
		print("Wrong number of parameters " + str((sys.argv)))
		return

	eventCount = 0
	with open('final', 'w') as outfile:
		for x in range(0,int(sys.argv[1])):
			with open("sim"+str(x)) as infile:
				for line in infile:
					if line[0] == "#":
						eventCount = eventCount + int(line[1:])
					else:
						outfile.write(line)
		outfile.write("#"+str(eventCount)+"\n")


main()