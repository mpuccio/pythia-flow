import os

def main():
	nJob = 10

	out_file = open("pythia.makeflow","w")
	out_file.write("PYTHIASETTINGS=xmldoc.tar\n")
	out_file.write("COMMAND="+os.getcwd()+"/command.cmnd\n")
	for x in range(0,nJob):
		out_file.write("log"+str(x)+": \n\t touch log"+str(x)+"\n")
		out_file.write("sim"+str(x)+": xmldoc.tar main42_new command.cmnd log"+str(x)+"\n\t")
		out_file.write("tar xf xmldoc.tar && ls -l >> log"+str(x)+"&& ./main42_new command.cmnd sim"+str(x)+" >> log"+str(x)+"\n")
	out_file.write("final: join.py ")
	for x in range(0,nJob):
		out_file.write(" sim"+str(x))
	out_file.write("\n\tpython join.py "+str(nJob)+"\n")

main()
