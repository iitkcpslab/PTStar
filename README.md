# PTStar
### **Files :**
**1. query.dat**  : Goal and Preference LTL queries must be specified in this file.
<pre>
Example file:
5 ---> Tuning Factor 'P' of the objective function.
2 ---> num of Preference LTL specs
./bin/ltl2tgba \--spin -C ' [ ](!p3) '
./bin/ltl2tgba \--spin -C ' [ ](!p4) '
./bin/ltl2tgba \--spin ' [](<>p1 && <>p2 && !p5) ' ---> Goal LTL spec
</pre>

**2. cfile_rec.dat** : 2D workspace descriptor file.
Input format:
nr - number of rows
nc - number of columns
nobs - no of obstacles
nobs coordinates - 'nobs' lines  mentioning the co-odinates of the obstacles
np - no of coordinates where a certain proposition is true
np coordinates - 'np' lines mentioning the coordinates and the proposition true at it.

Example file :
<pre>
10          -----> nr
10          -----> nc
5           -----> nobs
1 2         -----> 'nobs' lines (ex. this is an obstacle at coord (1,2) in the workspace)
3 4
5 6
9 5
3 9
4           -----> np
5 5 1       -----> 'np' lines (ex. at (5,5) , proposition 1 is true)
6 6 2
2 2 3
3 3 4
</pre>


**3. ./bin/ltl2tgba** : LTL to Buchi Automaton converter binary file. 
Install Spot-2.6 tool for LTL2TGBA converter. Copy the bin folder of spot installation to the current folder which contains ltl2tgba file. Check if the tool is working by running following command in the current folder using the command line:
<pre>
./bin/ltl2tgba \--spin ' [ ] (<>p && <>q) '
</pre>
This command should give us the Buchi automata transitions for the give query

**4. djk-optimal-run.cpp**: T&ast;-Preference ALgorithm for 2D workspace. This is the Dijkstras algorithm based solution to the mentioned problem.

**5. motion-planner-final.cpp** : T&ast;-Preference ALgorithm for 2D worksapce

**6. planner_variables.h** : Helper file for 2D workspace

----
### Execute Algorithm

**Compile**:  Compile djk-optimal-run.cpp and motion-planner-final.cpp using g++ command with flag -std=c++11 to generate the binaries djk-optimal-run and motion-planner-final(on Ubuntu OS) . 
For example -
g++ -std=c++11 djk-optimal-run.cpp -o djk-optimal-run
g++ -std=c++11 motion-planner-final.cpp -o motion-planner-final

**Execute**:  Execute the generated binaries using the following commands
./djk-optimal-run cfile_rec.dat
./motion-planner-final cfile_rec.dat

------------
