import os,sys
from math import sin,cos,pi
from gdml_parts_new import part1, part2, part3
from build_pmtinfo import build_pmtinfo

# This script generated GDML geometry files for KPIPE. It places the SiPMs parametrically.
# the generated portion is sandwiched between two parts (part1 and part2) below.


def generate_gdml_file( gdmlFilename, PMTINFOFilename, pmtsPerRing=14, numRings=7, pipeLength=344., pipeRadius=172.):
    """ Generates JSNS^2 GDML file. Populates detector with rings of SiPMs.

    For each SiPM, we lay down two components, the active and inactive SiPMs.
    We do this for Chroma. All units in cm.
    """

    pmtOffset = 8.2 		 #Displacement of PMTs so they completely lie in the correct volume
    lowerGlassOffset = 10.0 	 #Offset of PMT lower glass from PMT Glass
    baseOffset = 16.0		 #Offset of PMT Base from PMT Glass

    # PMTs around barrel
    zGap = pipeLength/float(numRings + 1)
    phiGap = 360.0/float(pmtsPerRing)
    barrelPMTDict = {} # dict: key=(sipmid,ringid,ring_sipmindex), value=(x,y,z,phi), where x,y,z is position, phi is rotation around z axis.
    PMT = 0
    for ring in xrange(0, numRings):
        z = -0.5*pipeLength + zGap*(ring+1)
        for ringPMT in xrange(0, pmtsPerRing):
            phi = ringPMT*phiGap
            x = pipeRadius*cos( phi*pi/180.0 )
            y = pipeRadius*sin( phi*pi/180.0 )
            x2 = (pipeRadius+pmtOffset)*cos( phi*pi/180.0 )
            y2 = (pipeRadius+pmtOffset)*sin( phi*pi/180.0 )
            rotphi = 90.0 + phi
            barrelPMTDict[ PMT ] = ( x, y, z, -rotphi, x2, y2, ring )
            PMT += 1

    # PMTs on the top and bottom
    # 10 on top and bottom in a 3, 4, 3 pattern
    endCapPMTDict = {}
    zTop = 0.5*pipeLength + pmtOffset
    zBottom = -0.5*pipeLength - pmtOffset
    xGap = (pipeRadius*2.0)/5.0
    xVals = [-xGap, 0.0, xGap, -1.5*xGap, -0.5*xGap, 0.5*xGap, 1.5*xGap, -xGap, 0.0, xGap]
    yVals = [xGap, xGap, xGap, 0.0, 0.0, 0.0, 0.0, -xGap, -xGap, -xGap]
    for i in range(0, len(xVals)):
	endCapPMTDict[PMT] = ( xVals[i], yVals[i], zTop, 180.0, xVals[i], yVals[i], zTop, i)
	endCapPMTDict[PMT+1] = ( xVals[i], yVals[i], zBottom, 0.0, xVals[i], yVals[i], zBottom, i)
	PMT += 2
 

    targetvol = "  <volume name=\"volOuterScintillator\">\n"
    targetvol+= "    <materialref ref=\"scintCocktailLAB\"/>\n"
    targetvol+= "    <solidref ref=\"outerScintillator\"/>\n"
    targetvol+= "    <physvol name=\"pvInnerTank\">\n"
    targetvol+= "      <volumeref ref=\"volInnerTank\"/>\n"
    targetvol+= "      <positionref ref=\"origin\"/>\n"
    targetvol+= "    </physvol>\n"
    keys = barrelPMTDict.keys()
    keys.sort()
    for key in keys:
        transform = barrelPMTDict[key]
        
	targetvol+="    <physvol name=\"pvPMTActiveArea%d\">\n"%(key)
        targetvol+="      <volumeref ref=\"volPMTActiveArea\"/>\n"
        targetvol+="      <position name=\"posPMTActiveArea%d\" unit=\"cm\" x=\"%.8f\" y=\"%.8f\" z=\"%.8f\"/>\n"%(key, transform[4], transform[5], transform[2] )
        targetvol+="      <rotation name=\"rotPMTActiveArea%d\" x=\"1.5707\" y=\"%.8f\" z=\"0.0\"/>\n"%(key, -transform[3]*pi/180.0)
        targetvol+="    </physvol>\n"

        targetvol+="    <physvol name=\"pvPMTGlassInactive%d\">\n"%(key)
        targetvol+="      <volumeref ref=\"volPMTGlassInactive\"/>\n"
        targetvol+="      <position name=\"posPMTGlassInactive%d\" unit=\"cm\" x=\"%.8f\" y=\"%.8f\" z=\"%.8f\"/>\n"%(key, transform[4], transform[5], transform[2] )
        targetvol+="      <rotation name=\"rotPMTGlassInactive%d\" x=\"1.5707\" y=\"%.8f\" z=\"0.0\"/>\n"%(key, -transform[3]*pi/180.0)
        targetvol+="    </physvol>\n"

        targetvol+="    <physvol name=\"pvPMTVacuum%d\">\n"%(key)
        targetvol+="      <volumeref ref=\"volPMTVacuum\"/>\n"
        targetvol+="      <position name=\"posPMTVacuum%d\" unit=\"cm\" x=\"%.8f\" y=\"%.8f\" z=\"%.8f\"/>\n"%(key, transform[4], transform[5], transform[2])
        targetvol+="      <rotation name=\"rotPMTVacuum%d\" x=\"1.5707\" y=\"%.8f\" z=\"0.0\"/>\n"%(key, -transform[3]*pi/180.0)
        targetvol+="    </physvol>\n"
        
	targetvol+="    <physvol name=\"pvPMTLowerGlass%d\">\n"%(key)
        targetvol+="      <volumeref ref=\"volPMTLowerGlass\"/>\n"
        targetvol+="      <position name=\"posPMTLowerGlass%d\" unit=\"cm\" x=\"%.8f\" y=\"%.8f\" z=\"%.8f\"/>\n"%(key, transform[4] + lowerGlassOffset*(transform[4]/(pipeRadius + pmtOffset)), transform[5] + lowerGlassOffset*(transform[5]/(pipeRadius+pmtOffset)), transform[2])
        targetvol+="      <rotation name=\"rotPMTLowerGlass%d\" x=\"1.5707\" y=\"%.8f\" z=\"0.0\"/>\n"%(key, -transform[3]*pi/180.0)
        targetvol+="    </physvol>\n"
       
	targetvol+="    <physvol name=\"pvPMTBase%d\">\n"%(key)
        targetvol+="      <volumeref ref=\"volPMTBase\"/>\n"
        targetvol+="      <position name=\"posPMTBase%d\" unit=\"cm\" x=\"%.8f\" y=\"%.8f\" z=\"%.8f\"/>\n"%(key, transform[4] + baseOffset*(transform[4]/(pipeRadius+pmtOffset)), transform[5] + baseOffset*(transform[5]/(pipeRadius+pmtOffset)), transform[2])
        targetvol+="      <rotation name=\"rotPMTBase%d\" x=\"1.5707\" y=\"%.8f\" z=\"0.0\"/>\n"%(key, -transform[3]*pi/180.0)
        targetvol+="    </physvol>\n"
 
    endKeys = endCapPMTDict.keys()
    endKeys.sort()
    for endKey in endKeys:
	transform = endCapPMTDict[endKey]

        targetvol+="    <physvol name=\"pvPMTActiveArea%d\">\n"%(endKey)
        targetvol+="      <volumeref ref=\"volPMTActiveArea\"/>\n"
        targetvol+="      <position name=\"posPMTActiveArea%d\" unit=\"cm\" x=\"%.8f\" y=\"%.8f\" z=\"%.8f\"/>\n"%(endKey, transform[4], transform[5], transform[2] )
        targetvol+="      <rotation name=\"rotPMTActiveArea%d\" x=\"%.8f\" y=\"0.0\" z=\"0.0\"/>\n"%(endKey, transform[3]*pi/180.0)
        targetvol+="    </physvol>\n"        

        targetvol+="    <physvol name=\"pvPMTGlassInactive%d\">\n"%(endKey)
        targetvol+="      <volumeref ref=\"volPMTGlassInactive\"/>\n"
        targetvol+="      <position name=\"posPMTGlassInactive%d\" unit=\"cm\" x=\"%.8f\" y=\"%.8f\" z=\"%.8f\"/>\n"%(endKey, transform[4], transform[5], transform[2] )
        targetvol+="      <rotation name=\"rotPMTGlassInactive%d\" x=\"%.8f\" y=\"0.0\" z=\"0.0\"/>\n"%(endKey, transform[3]*pi/180.0)
        targetvol+="    </physvol>\n"        

	targetvol+="    <physvol name=\"pvPMTVacuum%d\">\n"%(endKey)
        targetvol+="      <volumeref ref=\"volPMTVacuum\"/>\n"
        targetvol+="      <position name=\"posPMTVacuum%d\" unit=\"cm\" x=\"%.8f\" y=\"%.8f\" z=\"%.8f\"/>\n"%(endKey, transform[4], transform[5], transform[2] )
        targetvol+="      <rotation name=\"rotPMTVacuum%d\" x=\"%.8f\" y=\"0.0\" z=\"0.0\"/>\n"%(endKey, transform[3]*pi/180.0)
        targetvol+="    </physvol>\n"        
       
	targetvol+="    <physvol name=\"pvPMTLowerGlass%d\">\n"%(endKey)
        targetvol+="      <volumeref ref=\"volPMTLowerGlass\"/>\n"
        targetvol+="      <position name=\"posPMTLowerGlass%d\" unit=\"cm\" x=\"%.8f\" y=\"%.8f\" z=\"%.8f\"/>\n"%(endKey, transform[4], transform[5], transform[2] + lowerGlassOffset*(transform[2]/abs(transform[2])))
        targetvol+="      <rotation name=\"rotPMTLowerGlass%d\" x=\"%.8f\" y=\"0.0\" z=\"0.0\"/>\n"%(endKey, transform[3]*pi/180.0)
        targetvol+="    </physvol>\n"        

	targetvol+="    <physvol name=\"pvPMTBase%d\">\n"%(endKey)
        targetvol+="      <volumeref ref=\"volPMTBase\"/>\n"
        targetvol+="      <position name=\"posPMTBase%d\" unit=\"cm\" x=\"%.8f\" y=\"%.8f\" z=\"%.8f\"/>\n"%(endKey, transform[4], transform[5], transform[2] + baseOffset*(transform[2]/abs(transform[2])))
        targetvol+="      <rotation name=\"rotPMTBase%d\" x=\"%.8f\" y=\"0.0\" z=\"0.0\"/>\n"%(endKey, transform[3]*pi/180.0)
        targetvol+="    </physvol>\n"        
 
    targetvol+="  </volume>\n"

    fgdml = open( gdmlFilename, 'w' )
    print >> fgdml, part1+"\n"+targetvol+"\n"+part2+"\n"+part3+"\n"
    fgdml.close()

    pmtinfo = build_pmtinfo( PMT, barrelPMTDict, endCapPMTDict)
    fpmtinfo = open( PMTINFOFilename, 'w' )
    print >> fpmtinfo, pmtinfo+'\n'
    fpmtinfo.close()


if __name__=="__main__":
   
    pmtsPerRing = 14
    numRings = 7
    pipeLength = 344. # cm
    pipeRadius = 172. # cm
    generate_gdml_file( "jsns2new.gdml", "PMTINFO.ratdb", pmtsPerRing, numRings, pipeLength, pipeRadius)
        

    
