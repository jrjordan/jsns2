import StringIO
from ROOT import *
from array import array

def save_as_roottree( npmts, barrelPMTDict, endCapPMTDict):
    r = TFile('PMTINFO.root', 'recreate')
    t = TTree('pmtinfo','PMT Positions')
    opdetid = array('i',[0])
    x = array('f',[0.0])
    y = array('f',[0.0])
    z = array('f',[0.0])
    t.Branch( "opdetid", opdetid, "opdetid/I" )
    t.Branch( "x", x, "x/F" )
    t.Branch( "y", y, "y/F" )
    t.Branch( "z", z, "z/F" )
    for ipmt in xrange(0,npmts):
        opdetid[0] = ipmt
        if ipmt in barrelPMTDict:
            pmtdict = barrelPMTDict
	elif ipmt in endCapPMTDict:
	    pmtdict = endCapPMTDict

        x[0] = pmtdict[ipmt][0]
        y[0] = pmtdict[ipmt][1]
        z[0] = pmtdict[ipmt][2]
        t.Fill()
    r.Write()

def build_pmtinfo(npmts, barrelPMTDict, endCapPMTDict):
    """generates PMTINFO.db.
    
    This is needed because we hacked RAT.  It stil thinks we are using PMTs,
    so we have to give it pseudo-data. Fixing this is on a to-do list somewhere.
    We also take the opportunity to build a look up table of channel and position.

    """
    pmtinfo = StringIO.StringIO()
    print >> pmtinfo, "{"
    print >> pmtinfo, "  name:\"PMTINFO\","
    print >> pmtinfo, "  valid_begin: [0,0],"
    print >> pmtinfo, "  valid_end: [0,0],"
    xposlist = "["
    yposlist = "["
    zposlist = "["
    #dirXlist = "["
    #dirYlist = "["
    #dirZlist = "["
    typelist = "["
    for n in xrange(0,npmts):
        if n in barrelPMTDict:
            xposlist += " %.4f"%(barrelPMTDict[n][0])
            yposlist += " %.4f"%(barrelPMTDict[n][1])
            zposlist += " %.4f"%(barrelPMTDict[n][2])
	    #dirXlist += " %.4f"%(-barrelPMTDict[n][0])
	    #dirYlist += " %.4f"%(-barrelPMTDict[n][1])
	    #dirZlist += " %.4f"%(0)
	elif n in endCapPMTDict:
	    xposlist += " %.4f"%(endCapPMTDict[n][0])
            yposlist += " %.4f"%(endCapPMTDict[n][1])
            zposlist += " %.4f"%(endCapPMTDict[n][2])
	    #dirXlist += "%.4f"%(0)
	    #dirYlist += "%.4f"%(0)
	    #dirZlist += "%.4f"%(-endCapPMTDict[n][2])
            
        typelist += "1"
        if n!=npmts-1:
            xposlist+=","
            yposlist+=","
            zposlist+=","
	    #dirXlist += ","
	    #dirYlist += ","
	    #dirZlist += ","
            typelist+=","
    xposlist += "]"
    yposlist += "]"
    zposlist += "]"
    #dirXlist += "]"
    #dirYlist += "]"
    #dirZlist += "]"
    typelist += "]"
    print >> pmtinfo, " x:",xposlist,","
    print >> pmtinfo, " y:",yposlist,","
    print >> pmtinfo, " z:",zposlist,","
    #print >> pmtinfo, " dir_x:",dirXlist,","
    #print >> pmtinfo, " dir_y:",dirYlist,","
    #print >> pmtinfo, " dir_z:",dirZlist,","
    print >> pmtinfo," type:",typelist,","
    print >> pmtinfo,"}"

    save_as_roottree( npmts, barrelPMTDict, endCapPMTDict)
    return pmtinfo.getvalue()

if __name__ == "__main__":
    pmtinfo = build_pmtinfo( 200000 )
    print pmtinfo
