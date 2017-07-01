import btree
import ustruct
from ucollections import namedtuple

#this class is intended to be in micropython-lib/dbm module
class dbm:
    
    def __init__(self,file,psize=512,keyformat='>i', valformat='>i',dup=False):
        self.db=None
        self.f=None
        self.file = file
        self.psize = psize
        self.kb=bytearray(ustruct.calcsize(keyformat))
        self.vb=bytearray(ustruct.calcsize(valformat))
        self.keyformat=keyformat
        self.valformat=valformat
        self.vnt=None
        self.knt=None
        self.dup=dup
    
    def __enter__(self):
        try:
            self.f = open(self.file,'r+b')
            self.db = btree.open(self.f,pagesize=self.psize,flags=btree.DUPLICATE_KEY if self.dup is True else 0)
            return self
        except OSError:
            try:
                self.f = open(self.file,'w+b')
                self.db = btree.open(self.f,pagesize=self.psize,flags=btree.DUPLICATE_KEY if self.dup is True else 0)
                return self
            except OSError as e:
                print(repr(e))
                if self.f is not None:
                    self.f.close()
    
    def __exit__(self,exc_type,value,traceback):
        if self.db is not None:
            self.db.close()
        if self.f is not None:
            self.f.close()
 
    @staticmethod
    def pack_data(data,_format,buf):
        if isinstance(data,tuple):
            ustruct.pack_into(_format,buf,0,*data)
        else:
            ustruct.pack_into(_format,buf,0,data)

    def get_size(self):
        self.f.seek(0,2)
        size = self.f.tell()
        return size

    def __setitem__(self,key,val):
        dbm.pack_data(key,self.keyformat,self.kb)
        dbm.pack_data(val,self.valformat,self.vb)
        self.db[bytes(self.kb)]=bytes(self.vb)

    def ntuple(self,nt,data):
        if nt is not None:
            return nt(*data)
        else:
            return data

    def _iter(self,items):
        for i in items:
            yield self.ntuple(self.knt,ustruct.unpack_from(self.keyformat,i[0])),self.ntuple(self.vnt,ustruct.unpack_from(self.valformat,i[1]))

    def __getitem__(self,key):
        if isinstance(key,slice):
            dbm.pack_data(key.start,self.keyformat,self.kb)
            start=bytes(self.kb)
            dbm.pack_data(key.stop,self.keyformat,self.kb)
            stop=bytes(self.kb)
            items = self.db.items(start,stop)
            return iter(self._iter(items))
        else:
            dbm.pack_data(key,self.keyformat,self.kb)
            val = self.db[bytes(self.kb)]
            return self.ntuple(self.vnt,ustruct.unpack_from(self.valformat,val))

    def __iter__(self):
        return iter(self._iter(self.db.items()))
    
    def __delitem__(self,key):
        dbm.pack_data(key,self.keyformat,self.kb)
        del self.db[bytes(self.kb)]

    def set_key_ntuple(self,*field_names):
        self.knt = namedtuple('Key',field_names)
        return self.knt

    def set_val_ntuple(self,*field_names):
        self.vnt = namedtuple('Val',field_names)
        return self.vnt


def set_db(j):
    with dbm(f_name,keyformat=k_fmt,valformat=v_fmt,dup=dupplic) as d:
        for i in range(j):
            d[ i//dup_div+1, (i//dup_div)%100, 100-((i//dup_div)%100), ((i//dup_div)%100)*1010] = 20170000+100*((i//100)%12+1)+((i//100)%31)+1, i, (i//100)%2, 1, i%1000, 10, (i//10)%2

def read_db():
    with dbm(f_name,keyformat=k_fmt,valformat=v_fmt,dup=dupplic) as d:
        d.set_key_ntuple(*k_t)
        d.set_val_ntuple(*v_t)
        for i in d:
            print ("read:", i)

def read_at(k11=0, k12=0, k13=0, k14=0, k21=99999, k22=255, k23=99999, k24=9999999):
    with dbm(f_name,keyformat=k_fmt,valformat=v_fmt,dup=dupplic) as d:
        d.set_key_ntuple(*k_t)
        d.set_val_ntuple(*v_t)
        ii=1
        for i in d[(k11,):(k21,)]:
            if i[0].k2>=k12 and i[0].k2<=k22 and i[0].k3>=k13 and i[0].k3<=k23 and i[0].k4>=k14 and i[0].k4<=k24:
                print ("sel: ",ii,  i)
                ii+=1

def delete_at ():
    with dbm(f_name,keyformat=k_fmt,valformat=v_fmt,dup=dupplic) as d:
        d.set_key_ntuple(*k_t)
        d.set_val_ntuple(*v_t)
        ii=1
        t=(7,6,94,6060)
        print ("del: ",ii,t, d[t])
        del d[t]
        ii+=1

#-----------------------------------------------------------------	   
# main:

f_name = "file.db" 
k_fmt = "!HBHI"
k_t = ('k1', 'k2', 'k3', 'k4')
v_fmt = "!IIBBHHB"
v_t = ('v1', 'v2', 'v3', 'v4', 'v5', 'v6', 'v7')
dupplic=True  # True => set dbm to manage dupplicate keys
dup_div= 10  

with open(f_name, "wb") as f:
    pass

set_db(100)
read_db()
read_at(k11=7, k21=8) 

set_db(100)
read_db()
read_at(k11=7, k21=8) 

set_db(100)
read_db()
read_at(k11=7, k21=8) 

set_db(100)
read_db()
read_at(k11=7, k21=8) 

delete_at()
