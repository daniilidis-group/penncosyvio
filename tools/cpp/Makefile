#
# GRASP Lab UPenn
# Bernd Pfrommer 2016
#
CCC       = g++
CCC_FLAGS = -ansi -pedantic-errors -Wall -Wextra -Wno-unused-parameter -Wno-long-long 

LIBS = -lgsl -lcblas
COMMON_OBJ = Pose.o Trajectory.o Quaternion.o
TEST_OBJ   = test_trajectory.o TrajectoryTester.o
ALIGN_OBJ  = align_trajectory.o

ALL_OBJ    = $(COMMON_OBJ) $(TEST_OBJ) $(ALIGN_OBJ)
ALL_EXE    = test_trajectory align_trajectory

all: $(ALL_EXE)

test_trajectory: $(TEST_OBJ) $(COMMON_OBJ)
	g++ -o $@ $^ $(LIBS)

align_trajectory: $(ALIGN_OBJ) $(COMMON_OBJ)
	g++ -o $@ $^ $(LIBS)
clean:
	@rm -f *.o $(ALL_EXE) path_plot.txt
clobber: clean
	@rm -f *.d

#
# -------------------------------- general rules ---------------------------
%.o:		%.cpp
	$(CCC) -c $(CCC_FLAGS) -o $@ $<

#
# -------------------------------- dependency generation -------------------
#
%.d: %.cpp
	@set -e; $(CCC) -MM -MG $(CCC_FLAGS) $< \
		| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
		[ -s $@ ] || rm -f $@

-include $(ALL_OBJ:%.o=%.d)
