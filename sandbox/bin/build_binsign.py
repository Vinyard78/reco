#!/usr/bin/python
# Logiciel BigImBaz 2 propriete de l'INRIA - Confidentiel

# This program create the parameter file use to create binary signature
# EB Aug 2008: adapted to a single list of prefixes, and added search path for siftgeo module

import sys,os,time,math,thread
milpix = os.getenv('MILPIX')
sys.path.append(milpix + '/runtime')
from siftgeo2 import *

def make_initial_projection(d,nproj,nbvw):
  """ call octave to make a random projection """
  tmpfile=os.tempnam()
  p=os.popen("octave -q","w")
  p.write("""
  
function P = rand_ortho_proj (d, n)

  P = randn (d, d);

  for i = 1:d
    P (:, i) = P (:, i) ./ norm (P (:, i), 2);
  end

  [Q, R] = qr (P);

  P = Q (:, 1:n);

end


d=%d
nbrandproj=%d

rand(127654,123);
P=rand_ortho_proj(d,d);

f=fopen("%s","wb");
fwrite(f,[d,nbrandproj,0],"int32");
fwrite(f,P,"float");
fclose(f);

"""%(d,nproj,tmpfile))
  p.close() # this waits until octave quits => has done its job
  sb=siftgeo_binarize_read_p(tmpfile,nbvw)
  os.unlink(tmpfile)
  return sb


def dynprint(s):
  """ useful to monitor progress interactively """
  if os.isatty(1):
    print "%s    \r"%s,
    sys.stdout.flush()


class LogHist:
  """ log histogram in base 2. Each bin is between 2 powers of 2 """
  def __init__(self):
    self.l2=math.log(2)
    self.bins=[0]*64
    self.m=1L<<60
  def log2(self,x):
    if x==0: return 0
    return math.log(x)/self.l2
  def add(self,x):
    self.bins[int(self.log2(x))]+=1
    if x<self.m: self.m=x
  def __str__(self):
    s=''
    for i,b in enumerate(self.bins):
      if b!=0:
        s+='[%d,%d):%d '%(1<<i,1<<(i+1),b)
    s+="min=%d"%self.m
    return s



def make_vw_map(vwgeo_generator):
  """ load a set of vwgeo files and keep only the vw numbers in an ivec per file
  Returns a list of ivecs """
  
  vws=[]

  print "loading vwgeos"
  t0=time.time()
  nmissing=0  
  for fname in vwgeo_generator():
    dynprint("    %s"%fname[-40:])
    try:
      vws.append(vwgeo_read_vwidx(open(fname,"r"),-1))
    except IOError:
      nmissing+=1
      vws.append(None)

  t1=time.time()

  print "loaded %d vwgeos (%d missing) in %.3f s, total %d descs"%(
    len(vws),nmissing,t1-t0,sum([ivec_length(i) for i in vws if i!=None]))

  nbvw=max([ivec_max(i) for i in vws if i!=None])+1

  print "nbvw=%d"%nbvw
  
  return (nbvw,vws)


class RunOnSet:
  """ run function f(i) on all i in l with n threads """
  
  def __init__(self,n,l,f):
    if n==1:
      for i in l:
        f(i)
    else:
      (self.n,self.l,self.f)=(n,l,f)
      self.lock=thread.allocate_lock()
      self.lock2=thread.allocate_lock()
      self.lock2.acquire()
      for i in range(n):
        thread.start_new_thread(self.loop,())
      self.lock2.acquire()
      
  def loop(self):
    while True:
      self.lock.acquire()
      if self.l==[]:
        self.n-=1
        if self.n==0:
          self.lock2.release()
        self.lock.release()
        return
      i=self.l.pop()
      self.lock.release()
      self.f(i)

  
def compute_medians(sb,vw0,vw,pa,hist):
  descs=pa[vw-vw0]
  hist.add(descs.n)
  dynprint("    %d (min %d descs)"%(vw,hist.m))
  (fpoints,d2)=siftgeo_to_ffq(descs.pts,descs.n)
  siftgeo_binarize_fill_medians(sb,vw,fpoints,descs.n)
  free_ffq(fpoints)

  
def compute_medians_in_slice(sb,vw0,vw1,pa,hist,nn=1):
  """ compute medians for vw in range(vw0,vw1).
  pa[i-vw0] contains the learning points for visual word i"""
  t0=time.time()
  print "    computing medians (%d threads)"%nn
  RunOnSet(nn,range(vw0,vw1),lambda vw: compute_medians(sb,vw0,vw,pa,hist))   
  t1=time.time()
  print "    done in %.3f s, min nb of descs so far: %d"%(t1-t0,hist.m)




def make_binsign_computer(vwgeo_generator,siftgeo_generator,
                          nslice=1,d=128,nproj=64,nn=1):
  """ makes a binsign computer from a list of corresponding vwgeo and siftgeo files.
  The 2 lists are given by generators that return filenames, in the same order.
  nslice: if all the descriptors do not fit in ram, split the job in that many slices.
          Descriptors will be read nslice times
  d: # of dimensions (depends on descriptor)
  nproj: # of projections (= NB_BITS_VAL_BINSIGN in ivfgeo.h)
  """
  
  (nbvw,vws)=make_vw_map(vwgeo_generator)

  sb=make_initial_projection(d,nproj,nbvw)

  print "projection matrix: %d projections in %d dimensions"%(sb.nproj,sb.d)

  hist=LogHist()

  vwstep=nbvw/nslice
  
  for vw0 in range(0,nbvw,vwstep):
    vw1=min(vw0+vwstep,nbvw)

    print "  computing medians for slice %d-%d"%(vw0,vw1)

    pa=PointsetArray(vw1-vw0)

    # read all descs and keep those that are in range(vw0,vw1)
    print "    dispatching descs"
    t0=time.time()
    i=0
    for fname in siftgeo_generator():
      map=ivec_remap(vws[i],vw0,vw1)
      siftgeo_read_dispatch(fname,map,pa.this)
      ivec_delete(map)
      i+=1
      # dynprint("    %d"%i)
    t1=time.time()

    print "    %d descs loaded in %.3f s"%(
      sum([pa[vw-vw0].n for vw in range(vw0,vw1)]),t1-t0)
  
    compute_medians_in_slice(sb,vw0,vw1,pa,hist,nn=nn)

  print "nb descs histogram:",hist

  for i in vws:
    ivec_delete(i)

  return sb


class IvecArray:
  """Wraps a ivec*. The array owns the ivecs """

  def __init__(self,n):
    self.this=new_IvecArray(n)
    self.n=n
    for i in range(n):
      IvecArray_setitem(self.this,i,ivec_new_void())
      
  def __getitem__(self,i):
    assert 0<=i<self.n
    return IvecArray_getitem(self.this,i)

  def __del__(self):
    for i in range(self.n):
      ivec_delete(IvecArray_getitem(self.this,i))
    delete_IvecArray(self.this)


def make_binsign_computer_2stage(vwgeo_generator,siftgeo_generator,tmpdir,
                                 nslice=5,d=128,nproj=64,nn=1):

  """ same as make_binsign_computer, but does only one pass over the descriptors.
  The descriptors are stored in nslice files in a temp dir (makes sense only if the
  temp dir is big enough to hold all the descriptors && is faster than the
  siftgeo directory, eg. a local scratch directory)"""
  
  (nbvw,vws)=make_vw_map(vwgeo_generator)

  sb=make_initial_projection(d,nproj,nbvw)

  print "projection matrix: %d projections in %d dimensions"%(sb.nproj,sb.d)
  hist=LogHist()
  vwstep=(nbvw+nslice-1)/nslice

  assert (nbvw-1)/vwstep<nslice

  print "Dispatching descs in %d slices (vwstep=%d)"%(nslice,vwstep)
  t0=time.time()
  
  fs=[open("%s/slice_%d.tmp"%(tmpdir,i),"w") for i in range(nslice)]
  slice_to_vw=IvecArray(nslice)
  
  totsz=0
  pa=PointsetArray(nslice)
  
  i=0
  for fname in siftgeo_generator():
    vwl=vws[i]
    if vwl:
      ivec_div_rem(vwl,slice_to_vw.this,vwstep)
      # print fname,ivec_length(vwl)    
      siftgeo_read_dispatch(fname,vwl,pa.this)
      totsz+=ivec_length(vwl)
      ivec_delete(vwl)
    if totsz>8*1000*1000:
      print "\n  flush slices %.3f s"%(time.time()-t0)
      for j in range(nslice):
        ps=pa[j]
        write_points_file(fs[j],ps.pts,ps.n,0)
        pa[j]=pointset_t()
      totsz=0
    i+=1
    dynprint("  %d"%i)

  for i in range(nslice):
    ps=pa[i]
    write_points_file(fs[i],ps.pts,ps.n,0)    
    fs[i].close()

  del pa
  
  t1=time.time()
  print "Done in %.3f s"%(t1-t0)

  print "Handling slices"

  for i in range(nslice):
    vw0=i*vwstep
    vw1=min(vw0+vwstep,nbvw)

    fname="%s/slice_%d.tmp"%(tmpdir,i)
    print "  slice %d: vw %d-%d (from %s)"%(i,vw0,vw1,fname)
    pa=PointsetArray(vw1-vw0)

    # reload the slice
    t0=time.time()
    siftgeo_read_dispatch(fname,slice_to_vw[i],pa.this)
    os.unlink(fname)
    t1=time.time()
    
    print "    loaded %d pts in %.3f s"%(
      sum([pa[i-vw0].n for i in range(vw0,vw1)]),t1-t0)

    compute_medians_in_slice(sb,vw0,vw1,pa,hist,nn=nn)

  print "nb descs histogram:",hist


  return sb


def filelist_generator(fl,ext):
  for l in open(fl,"r"):
    yield l.strip() + ext


# Main Program

if __name__=="__main__":
  prefixlist = sys.argv[1]
  outfile = sys.argv[2]
  sb=make_binsign_computer(lambda: filelist_generator(prefixlist,".vwgeo"),
                           lambda: filelist_generator(prefixlist,".siftgeo"))
  print "storing output to",outfile
  siftgeo_binarize_write(outfile,sb)
  siftgeo_binarize_delete(sb)
