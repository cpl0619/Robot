"""Fully fledged forwards kinematic simulator /RunAngles/, input is the motor angles & output is the end-effector position
Before those are helper functions, mainly items that deal with vectors properly -Nathan W."""

from math import sin, cos, pi

def add(x, y):
  return [x[ind]+y[ind] for ind in range(len(x))]
def scale(x, c):
  return [x1*c for x1 in x]

def dot_product(x, y):
  return sum([x[ind]*y[ind] for ind in range(len(x))])

def cross_product(x,y):
  return([x[1]*y[2] - x[2]*y[1],
          x[2]*y[0] - x[0]*y[2],
          x[0]*y[1] - x[1]*y[0]])
def norm(x):
  mag = sum(c**2 for c in x)**0.5
  return([c / mag for c in x])
def rotVectorAxis(vector, axis, angle):
  cross = cross_product(axis,vector)
  dot = dot_product(axis, vector)
  return [v * cos(angle) + c * sin(angle) + a * dot * (1 - cos(angle)) for v, c, a in zip(vector, cross, axis)]

def prnt(x):
  hold = '(' + ','.join(f"{c:.5f}" for c in x) + ')'
  print(hold)

def RunAngles(turns,printOpt = False): #Might need to scale based on units...
  forward = [0,0,1]
  right   = [1,0,0]
  up      = [0,1,0]
  pos     = [0,0,.259]

  posTracked = [pos[:]] #ForOutput
  #TURN 1
  #Input Rotation
  up = norm(rotVectorxis(up, forward, turns[0]))
  right = norm(rotVectorxis(right, forward, turns[0]))
  #Forced Rotation
  forward = norm(rotVectorxis(forward, right, pi/2))
  up = norm(rotVectorxis(up, right, pi/2))

  #TURN 2
  #Input Rotation
  upTemp = norm(rotVectorxis(up, forward, turns[1]+pi/2))
  rightTemp = norm(rotVectorxis(right, forward, turns[1]+pi/2))
  #Forced Rotation
  forwardTemp = norm(rotVectorxis(forward, rightTemp, pi/2))
  pos = add(pos, forwardTemp)
  posTracked.append(pos)

  #TURN 3
  #Input Rotation
  up = norm(rotVectorxis(up, forward, turns[1]+turns[2]+pi/2))
  right = norm(rotVectorxis(right, forward, turns[1]+turns[2]+pi/2))
  #Forced Rotation
  forward = norm(rotVectorxis(forward, right, pi/2))
  up = norm(rotVectorxis(up, right, pi/2))
  pos = add(pos, forward)
  posTracked.append(pos)

  #TURN 4
  #Input Rotation
  up = norm(rotVectorxis(up, forward, turns[3]))
  right = norm(rotVectorxis(right, forward, turns[3]))
  #Forced Rotation
  forward = norm(rotVectorxis(forward, right, pi/2))
  up = norm(rotVectorxis(up, right, pi/2))

  #TURN 5
  #Input Rotation
  up = norm(rotVectorxis(up, forward, turns[4]+pi/2))
  right = norm(rotVectorxis(right, forward, turns[4]+pi/2))
  #Forced Rotation
  forward = norm(rotVectorxis(forward, right, pi/2))
  up = norm(rotVectorxis(up, right, pi/2))
  pos = add(pos, forward)
  posTracked.append(pos)


  #print(forward,up,right)
  if printOpt:
    for pos in posTracked:
      prnt(pos)
  return(posTracked)



from random import random
def optConvex(sizeX,func,start=[],step=.001,errorBound = 0):
  #Can't start from a multiple of pi due to cycling issues
  if start == []: start = optRandom(sizeX,func)[0]
  best = func(start)
  error = errorBound+1
  while error > errorBound:
    error = 0
    for ind in range(sizeX):
      new = start[:]
      new[ind] += step
      while func(new) < best:
        error += best-func(new)
        best = func(new)
        new[ind] += step
      new[ind] -= 2*step
      while func(new) < best:
        error += best-func(new)
        best = func(new)
        new[ind] -= step
      new[ind] += step
      start = new[:]
  if start[2] > pi: start[0],start[1],start[2] = (start[0]+pi)%(2*pi), pi-start[1], 2*pi-start[2] 
  flag = sum([-min(c[2],0) for c in RunAngles(x)]) < .0001
  return(start,best,flag)

def optRandom(sizeX,func,start=[],itrs = 10000):
  #Can't start from a multiple of pi due to cycling issues
  if start == []: start = [1]*sizeX
  best = func(start)
  for c in range(itrs):
    new = [random()*pi*2 for c in range(sizeX)]
    if func(new) < best:
      best = func(new)
      start = new[:]
  if start[2] > pi: start[0],start[1],start[2] = (start[0]+pi)%(2*pi), pi-start[1], 2*pi-start[2] 
  return(start,best)

def dist(x):
  return(sum([c**2 for c in x])**(1/2))
def testFun(x): #TargetDir to be implemented, only first 3 axis needed to set pos.
  TargetPos = [1,0,0]
  TargetPos = [-c for c in TargetPos]

  output = RunAngles(x)
  return(dist(add(TargetPos,output[-2]))+sum([-min(c[2],0) for c in output]))

#TEST FUNCTIONING
sol = optRandom(5,testFun)
print(sol)
RunAngles(sol[0],True)
