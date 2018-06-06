function Point(x,y){
     this.x=x;
     this.y=y;
}
Point.prototype.show=function(){
  print('(x,y) = '+this.x+','+this.y);
}

Point.prototype.z = 1000;