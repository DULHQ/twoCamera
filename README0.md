# Homework of ThoughtWorks

标签 ： 开发手册

---

### 1.项目描述

#### 1.1 项目名称<br>
　　渲染道路网格。
#### 1.2 项目需求<br>
　　根据迷宫道路网格的大小及连通性，给出渲染过后的渲染网格。每格道路"[R]"在初始状态时都被墙"[W]"包围，连通后，两
  个道路格之间的墙"[W]"变成道路"[R]"。<br>
#### 1.3 功能列表<br>

    /**
	 * 
	 * @author hq
	 * @param num
	 * @param low
	 * @param high 
	 * @return true or false
	 *          当num位于[low, high]区间内返回true；否则，false
	 * 
	 */
**private boolean isOutOfRange(int num, int low, int high)；**

    /**
	 * 
	 * @author hq
	 * @param str
	 * @param sp
	 *         分隔符
	 * @param len 
	 * 		   str经过sp分隔后得到的字符串数组长度 
	 * @return the array of ints
	 * 		　  str经过sp分隔后得到的字符串数组,转换得到的int数组  
	 * 
	 */
**private int[] getIntFromString(String str, String sp, int len)；**

    /**
	 * 
	 * @author hq
	 * @param rows
	 * @param cols
	 * @return the list of strings
	 * 			list有（2*rows+1）行，
	 * 				偶数行字符串: "[W]" + " [W] [W]" * cols，
	 * 				奇数行字符串："[W]" + " [R] [W]" * cols
	 * 
	 */
**private List\<String> initGrid(int rows, int cols);**

    /**
	 * 
	 * @author hq
	 * @param str
	 * @param pos
	 * 			需要设置联通的位置
	 * @return string
	 * 			连通后的字符串str
	 * 
	 */
**private String setConn(String str, int pos);**

    /**
	 * 
	 * @author hq
	 * @param size
	 * @param Conn
	 * @param renderGrid
	 * 			初始状态，还未连通过的渲染网格
	 * @return the list of string
	 * 			连通后的renderGrid
	 * 
	 */
**private List\<String> connRenderGrid(final int[] size, final String Conn, List<String> renderGrid);**

    /**
	 * 
	 * @author hq
	 * @param Size
	 * @param Conn
	 * @return the list of strings; null if error
	 * 
	 */
**private List\<String> getRenderGrid(final String Size, final String Conn);**　　　　　
### 2. 开发环境
#### 2.1 开发环境
　系统： Ubuntu16.04<br>
　语言： Java<br>
　JDK版本：1.8.0_141<br>
#### 2.2 运行命令
　javac Road2Render.java<br> 
　java Road2Render<br>
#### 2.3 输入格式
　第一行输入道路网格：<br>
　　m n表示m行n列的道路网格，**m和n均是整数且在[1,1000]范围内**<br>
　第二行输入道路连通信息：<br>
　　"0,1 0,2"表示cell（0,1）和cell（0,2）是连通的，多个连通之间用'；'分隔<br>
　输入示例：<br>
 ```
 3 3
 0,1 0,2;0,0 1,0;0,1 1,1;0,2 1,2;1,0 1,1;1,1 1,2;1,1 2,1;1,2 2,2;2,0 2,1
 ```
#### 2.4 错误警告
　命令格式错误： "Incorrect command format."<br>
 　字符超限错误： "Number out of range."<br>
 　非法字符错误： "Invalid number format."<br>
 　连通性错误：   "Maze format error."<br>
### 3. 测试Demo
#### ３.１ 命令格式测试
出错场景：<br>
　1.道路网格和单个cell的行列值：<br>
　　　不输入或只输入空格<br>
　　　只输入行或只输入列，不带、带一个或多个空格<br>
　　　输入多个行或输入多个列，数字间带多个空格<br>
　2.连通<br>
　　　单个cell的行列不用或用多个','分隔<br>
　　　两个连通的cell之间不用或用多个' '分隔<br>
　　　多个连接之间不用或用多个';'分隔<br>
**输入1：**<br>
```
  3
 0,1 0,2;
 ```
**输出1：**<br>
```
Incorrect command format.  3
```
**输入2：**<br>
```
3 3
0,1 0,2,0,1 1,1;
```
**输出2：**<br>
```
Incorrect command format. 0,1 0,2,0,1 1,1
```
#### 3.2 字符超限测试
出错场景：<br>
　１.道路网格的行列值(m,n)：m < 1 || n < 1 || m > 1000 || n > 1000<br>
　２.单个cell的行列值(i,j)： i < 0 || j < 0 || i >= m || j >= m<br>
**输入1**<br>
```
0 3
0,1 0,2;
```
**输出1**
```
Number out of range. 0
```
**输入2**<br>
```
3 -1
001,002 01,04;
```
**输出2**<br>
```
Number out of range. 3 -1
```
**输入3**<br>
```
3 3
001,002 01,04
```
**输出3**<br>
```
Number out of range. 4
```
#### 3.3 非法字符测试
出错场景：<br>
　　输入的行或列的字符不在'0'~'9'之间，且不等于'-'<br>
**输入1**<br>
```
a 3
0,1 0,2;
```
**输出1**<br>
```
Invalid number format. a 3
```
**输入2**<br>
```
3 3
00a,02 01,02
```
**输出2**<br>
```
Invalid number format. 00a,02
```
#### 3.4 连通性测试
出错场景：<br>
　　两个连通的cell1(r1,c1),cell2(r2,c2)，不满足(r1 == r2 && |c1-c2| == 1) || (c1 == c2 && |r1-r2| == 1)<br>
**输入1**<br>
```
3 3
0,1 1,0;0,1 0,2;
```
**输出1**<br>
```
Maze format error. 0,1 1,0
```
**输入2**<br>
```
3 3
0,1 0,2;0,0 1,0;0,1 1,1;0,2 1,2;1,0 1,1;1,1 1,2;1,1 2,1;1,2 2,2;2,0 2,1
```
**输出2**<br>
```
connected Render Grid is:
[W] [W] [W] [W] [W] [W] [W]
[W] [R] [W] [R] [R] [R] [W]
[W] [R] [W] [R] [W] [R] [W]
[W] [R] [R] [R] [R] [R] [W]
[W] [W] [W] [R] [W] [R] [W]
[W] [R] [R] [R] [W] [R] [W]
[W] [W] [W] [W] [W] [W] [W]
```

## 联系方式
email: ieryca@13.com<br>
qq: 1639542944<br>
