
# coding: utf-8

# In[1]:


from pyspark import SparkContext,SparkConf
from operator import add


# In[2]:


path = "data.txt"
sc = SparkContext("local", "Assignment")
edges = sc.textFile("data.txt").map(lambda x: tuple(map(int, x.split(' '))))


# In[3]:


def reducer(t):
    op = []
    (u, l) = t
    for v in l:
        op.append((v,rootprime[int(v)-1]))
    return op


# In[4]:


root = edges.flatMap(lambda r: [(r[0], r[1])]).sortBy(lambda a: int(a[0])).map(lambda r: r[1])
rootprime =root.collect()
while(True):
    newroot = root.map(lambda a : rootprime[int(a)-1])
    if(newroot.map(lambda t: t).reduce(add) == root.map(lambda t: t).reduce(add)):
        break;
    root = newroot
rootprime =root.collect()
roots = edges.flatMap(lambda r: [(r[1], r[0])])
neighbour = roots.groupByKey().mapValues(list)
output= neighbour.flatMap(reducer)


# In[5]:


output.map(lambda t: ' '.join(map(str, t))).saveAsTextFile('output')


# In[6]:


sc.stop()

