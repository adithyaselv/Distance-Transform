# Distance-Transform
Distance Transform computed using Meijster distance algorithm. 

### Link:
[http://fab.cba.mit.edu/classes/S62.12/docs/Meijster_distance.pdf](http://fab.cba.mit.edu/classes/S62.12/docs/Meijster_distance.pdf)

### Dependencies:
* OpenCV

### Compilation:
#### Mac:
Using Package config  
```
g++ dt.cpp -o dt `pkg-config --cflags --libs opencv`
```
### Running instructions:

```
 ./dt <imageName> <Method>
  
  Methods Available :
  EDT- Euclidean distance transform
  MDT- Manhattan distance transform
  CDT- Chessboard distance transform
  
  Example:
 ./dt 4_1_map.png EDT
