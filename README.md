Champsim Simulator:

Create a new folder Name it: dpc3_traces and put the downloaded trace files in it.

For granting permissions, run for both build_champsim.sh and run_champsim.sh

chmod +x script.sh             example :     chmod +x build_champsim.sh
sed -i -e 's/\r$/\n/' script.sh  example:      sed -i -e 's/\r$/\n/' build_champsim.sh

      3.	For building:  ./build_champsim.sh bimodal no no no no lru 1

      4.	For running:   ./run_champsim.sh bimodal-no-no-no-no-lru-1core 1 10 gcc_13B.trace.xz

      5.	For code change,
Look at replacement folder
Look at src folder: mainly cache.cc
Look at inc folder: mainly cache.h, block.h
