########################################
##
## Makefile
## LINUX compilation 
##
##############################################





#FLAGS
C++FLAG = -g -std=c++11

MATH_LIBS = -lm

EXEC_DIR=.


.cc.o:
	g++ $(C++FLAG) $(INCLUDES)  -c $< -o $@


#Including
INCLUDES=  -I. 

#-->All libraries (without LEDA)
LIBS_ALL =  -L/usr/lib -L/usr/local/lib 


#First Program (ListTest)

Cpp_OBJ_1=image.o h1.o

PROGRAM_1_NAME=h1

Cpp_OBJ_2=image.o h2.o

PROGRAM_2_NAME=h2

Cpp_OBJ_3=image.o h3.o

PROGRAM_3_NAME=h3

Cpp_OBJ_4=image.o h4.o

PROGRAM_4_NAME=h4

$(PROGRAM_1_NAME): $(Cpp_OBJ_1)
	g++ $(C++FLAG) -o $(EXEC_DIR)/$@ $(Cpp_OBJ_1) $(INCLUDES) $(LIBS_ALL)

$(PROGRAM_2_NAME): $(Cpp_OBJ_2)
	g++ $(C++FLAG) -o $(EXEC_DIR)/$@ $(Cpp_OBJ_2) $(INCLUDES) $(LIBS_ALL)

$(PROGRAM_3_NAME): $(Cpp_OBJ_3)
	g++ $(C++FLAG) -o $(EXEC_DIR)/$@ $(Cpp_OBJ_3) $(INCLUDES) $(LIBS_ALL)

$(PROGRAM_4_NAME): $(Cpp_OBJ_4)
	g++ $(C++FLAG) -o $(EXEC_DIR)/$@ $(Cpp_OBJ_4) $(INCLUDES) $(LIBS_ALL)

all: 
	make $(PROGRAM_1_NAME) $(PROGRAM_2_NAME) $(PROGRAM_3_NAME) $(PROGRAM_4_NAME)


clean:
	(rm -f *.o; rm image_demo)

(:
