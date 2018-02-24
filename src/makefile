################################################################################
# Automatically-generated file. Do not edit!
################################################################################

#calib_two:calib_two.cpp
#	g++ `pkg-config --cflags opencv` -o calib_two calib_two.cpp `pkg-config --libs opencv` `pkg-config --cflags gtk+-2.0`


PROGRAM=main


# 指定编译器
CC=g++
# 指定编译选项
CFLAGS=`pkg-config --cflags opencv` `pkg-config --cflags gtk+-2.0`
# 指定连接时的库
LDADD=`pkg-config --libs opencv` `pkg-config --libs gtk+-2.0`

# 指定删除命令及选项
RM=rm -rf

# 默认目标，创建可执行程序
all:${PROGRAM}

${PROGRAM}:main.cpp calib.cpp
	${CC} ${CFLAGS} -o ${PROGRAM} $^ ${LDADD}

.PHNOY:clean run

# 清除工作
clean:
	-${RM} ${PROGRAM} *.o

# 运行程序
run:
	./${PROGRAM}