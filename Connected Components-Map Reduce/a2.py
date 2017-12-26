# Issac Koshy
# Panicker
# issackos

# coding: utf-8

# In[1646]:


from pyspark import SparkContext
import networkx as nx
import sys


# In[1648]:


# Read inputgraph
path = sys.argv[1]
sc = SparkContext("local", "Simple App")
data = sc.textFile(path).cache()
inputgraph=list(data.map(lambda line: line.split()).collect())


# In[1641]:


# Starting of large star **
def Largestar(Edgelist):
    G = nx.Graph()
    G.add_edges_from(Edgelist)
    def N(node):
        path_lengths = nx.single_source_dijkstra_path_length(G, node)
        return [node for node, length in path_lengths.items() if length == 1]
    edges = sc.parallelize(Edgelist).flatMap(lambda record: [(record), (record[1], record[0])])
    neighbour = sorted(edges.groupByKey().mapValues(list)
    list1=[]
    for item in edges.collect():
        if(item[0]<item[1]):      
            m=min(item[0].split()+(N(item[0])))
            list1.append((item[1],m))
        else:
            list1.append((item[0],item[1]))
    return list1


# In[1642]:


# Starting of Small star **
def Smallstar(Edgelist):
    G = nx.Graph()
    G.add_edges_from(Edgelist)
    def N(node):
        path_lengths = nx.single_source_dijkstra_path_length(G, node)
        return [node for node, length in path_lengths.items() if length == 1]
    list2=sc.parallelize(Edgelist).map(lambda record:((record[0],record[1]) if record[1]<record[0] else (record[1],record[0]))).collect()
    list3=[]
    for item in list2:
        m=min(item[0].split()+(N(item[0])))
        if(item[1]<item[0])&(m!=item[1]):
            m=min(item[0].split()+(N(item[0])))
            list3.append((item[1],m))
        else:
            list3.append((item[0],m))    
    return list3


# In[1643]:


# Main loop, it will iterate till convergence
count=0
ls = [0]
ss = [1]
while not (set(ls)==set(ss)):
    ls=Largestar(inputgraph)
    ss=Smallstar(ls)
    inputgraph=ss
    count=count+1


# In[1645]:


# Write output file
file = open('output.txt','w') 
for item in sorted(set(ls),key=lambda x: x[1]):
    file.write(str(item[0])+' '+str(item[1])+'\n')
file.close()


