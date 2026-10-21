"""Fully fledged forwards kinematic simulator /RunAngles/, input is the motor angles & output is the end-effector position
Before those are helper functions, mainly items that deal with vectors properly -Nathan W."""

from math import sin, cos, pi

def add(x, y):
  return [x[ind]+y[ind] for ind in range(len(x))]

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

def RunAngles(turns):
  forward = [0,0,1]
  right   = [1,0,0]
  up      = [0,1,0]
  pos     = [0,0,0]

  for angle in turns:
    #Input Rotation
    up = norm(rotVectorAxis(up, forward, angle))
    right = norm(rotVectorAxis(right, forward, angle))
    #Forced Rotation
    forward = norm(rotVectorAxis(forward, right, pi/2))
    up = norm(rotVectorAxis(up, right, pi/2))


    pos = add(pos, forward)
    #print(forward,up,right)
    #prnt(pos)
  return(pos)

