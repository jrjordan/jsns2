import os

part1="""<?xml version="1.0" encoding="UTF-8"?>
<gdml xmlns:gdml="http://cern.ch/2001/Schemas/GDML"	
      xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
      xsi:noNamespaceSchemaLocation="http://service-spi.web.cern.ch/service-spi/app/releases/GDML/schema/gdml.xsd" >

<define>
  <position name="photocathode_offset" unit="cm" x="0" y="0" z="0.5*22.0-0.5*0.2"/>
  <position name="PMTBaseOffset" unit="cm" x="0" y="0" z="-7.0"/> 
  <position name="PMTPhotocathodeOffset" unit="cm" x="0" y="0" z="8.0"/>
  <position name="origin" unit="cm" x="0" y="0" z="0"/>
  <position name="PMT_offset" unit="cm" x="0" y="13.0" z="0"/>
  <rotation name="identity"/>
</define>

<materials>
  <element name="bromine" formula="Br" Z="35"> <atom value="79.904"/> </element>
  <element name="hydrogen" formula="H" Z="1">  <atom value="1.0079"/> </element>
  <element name="nitrogen" formula="N" Z="7">  <atom value="14.0067"/> </element>
  <element name="oxygen" formula="O" Z="8">  <atom value="15.999"/> </element>
  <element name="aluminum" formula="Al" Z="13"> <atom value="26.9815"/>  </element>
  <element name="silicon" formula="Si" Z="14"> <atom value="28.0855"/>  </element>
  <element name="carbon" formula="C" Z="6">  <atom value="12.0107"/>  </element>
  <element name="potassium" formula="K" Z="19"> <atom value="39.0983"/>  </element>
  <element name="chromium" formula="Cr" Z="24"> <atom value="51.9961"/>  </element>
  <element name="iron" formula="Fe" Z="26"> <atom value="55.8450"/>  </element>
  <element name="nickel" formula="Ni" Z="28"> <atom value="58.6934"/>  </element>
  <element name="calcium" formula="Ca" Z="20"> <atom value="40.078"/>   </element>
  <element name="magnesium" formula="Mg" Z="12"> <atom value="24.305"/>   </element>
  <element name="sodium" formula="Na" Z="11"> <atom value="22.99"/>    </element>
  <element name="titanium" formula="Ti" Z="22"> <atom value="47.867"/>   </element>
  <element name="argon" formula="Ar" Z="18"> <atom value="39.9480"/>  </element>
  <element name="boron" formula="B" Z="5"> <atom value="10.811"/> </element>  
  
  <material Z="1" formula=" " name="Vacuum">
    <D value="1.e-25" unit="g/cm3"/>
    <atom value="1.0079"/>
  </material>

  <material name="stainless_steel" formula="stainless_steel">
    <D value="7.9300" unit="g/cm3"/>
    <fraction n="0.0010" ref="carbon"/>
    <fraction n="0.1792" ref="chromium"/>
    <fraction n="0.7298" ref="iron"/>
    <fraction n="0.0900" ref="nickel"/>
  </material>
  
  <material formula=" " name="air">
    <D value="0.001205" unit="g/cc"/>
    <fraction n="0.781154" ref="nitrogen"/>
    <fraction n="0.209476" ref="oxygen"/>
    <fraction n="0.00937" ref="argon"/>
  </material>
  
  <material formula=" " name="Dirt">
    <D value="1.7" unit="g/cc"/>
    <fraction n="0.438" ref="oxygen"/>
    <fraction n="0.257" ref="silicon"/>
    <fraction n="0.222" ref="sodium"/>
    <fraction n="0.049" ref="aluminum"/>
    <fraction n="0.019" ref="iron"/>
    <fraction n="0.015" ref="potassium"/>
  </material>
  
  <material formula=" " name="mineral_oil">
    <D value="0.77" unit="g/cc"/>
    <fraction n="0.8563" ref="carbon"/>
    <fraction n="0.1437" ref="hydrogen"/>
  </material>

  <material formula=" " name="pseudocumene">
    <D value="0.8758" unit="g/cc"/>
    <fraction n="0.8994" ref="carbon"/>
    <fraction n="0.1006" ref="hydrogen"/>
  </material>
  
  <material formula=" " name="PPO">
    <D value="1.06" unit="g/cc"/>
    <fraction n="0.8142" ref="carbon"/>
    <fraction n="0.0501" ref="hydrogen"/>
    <fraction n="0.0633" ref="nitrogen"/>
    <fraction n="0.0723" ref="oxygen"/>
  </material>

  <material formula=" " name="C15H24_LAB">
    <D value="0.867" unit="g/cc"/>
    <fraction n="0.8815" ref="carbon"/>
    <fraction n="0.1185" ref="hydrogen"/>
  </material>

  <material formula=" " name="C16H26_LAB">
    <D value="0.867" unit="g/cc"/>
    <fraction n="0.8799" ref="carbon"/>
    <fraction n="0.1201" ref="hydrogen"/>
  </material>

  <material formula=" " name="C17H28_LAB">
    <D value="0.867" unit="g/cc"/>
    <fraction n="0.8785" ref="carbon"/>
    <fraction n="0.1215" ref="hydrogen"/>
  </material>

  <material formula=" " name="C18H30_LAB">
    <D value="0.867" unit="g/cc"/>
    <fraction n="0.8772" ref="carbon"/>
    <fraction n="0.1228" ref="hydrogen"/>
  </material>

  <material formula=" " name="C19H32_LAB">
    <D value="0.867" unit="g/cc"/>
    <fraction n="0.8761" ref="carbon"/>
    <fraction n="0.1239" ref="hydrogen"/>
  </material>

  <material formula=" " name="LAB">
    <D value="0.867" unit="g/cc"/>
    <fraction n="0.012" ref="C15H24_LAB"/>
    <fraction n="0.204" ref="C16H26_LAB"/>
    <fraction n="0.432" ref="C17H28_LAB"/>
    <fraction n="0.334" ref="C18H30_LAB"/>
    <fraction n="0.018" ref="C19H32_LAB"/>
  </material>

  <material formula=" " name="bisMSB">
    <D value="1.08" unit="g/cc"/>
    <fraction n="0.929" ref="carbon"/>
    <fraction n="0.071" ref="hydrogen"/>
  </material>
  
  <material formula=" " name="scintCocktailLAB">
    <D value="0.870" unit="g/cc"/>
    <fraction n="0.996552" ref="LAB"/>
    <fraction n="0.003431" ref="PPO"/>
    <fraction n="0.000017" ref="bisMSB"/>
  </material>
  
  <material formula=" " name="scintillator">
    <D value="0.78" unit="g/cc"/>
    <fraction n="0.996984" ref="mineral_oil"/>
    <fraction n="0.001919" ref="pseudocumene"/>
    <fraction n="0.001097" ref="PPO"/>
  </material>

  <material formula=" " name="chip_silicon">
    <D value="2.3" unit="g/cc"/>
    <fraction n="1.0" ref="silicon"/>
  </material>

  <material formula=" " name="borosilicate_glass">
    <D value="2.23" unit="g/cc"/>
    <fraction n="0.05238" ref="silicon"/>
    <fraction n="0.55873" ref="oxygen"/>
    <fraction n="0.38889" ref="boron"/>
  </material>

  <material formula=" " name="photocathode">
    <D value="5.0" unit="g/cc"/>
    <fraction n="1.0" ref="potassium"/>
  </material>

  <material formula=" " name="acrylic_uvt_good">
    <D value="1.14" unit="g/cc"/>
    <fraction n="0.0703" ref="hydrogen"/>
    <fraction n="0.558" ref="carbon"/>
    <fraction n="0.3717" ref="oxygen"/>
  </material>

</materials>

<solids>

  <box name="world"
       lunit="m"
       x="40.0"
       y="40.0"
       z="200.0" />
  <tube name="scintillatorTarget"
	lunit="m"
	rmax="1.70"
	z="3.40"
	deltaphi="360"
	aunit="deg"/>
  <tube name="innerTank"
	lunit="m"
	rmax="1.72"
	z="3.44"
	deltaphi="360"
	aunit="deg"/>
  <tube name="outerScintillator"
	lunit="m"
	rmax="2.2"
	z="4.40"
	deltaphi="360"
	aunit="deg"/>
  <tube name="outerTank"
        lunit="m"
	rmax="2.22"
	z="4.44"
	deltaphi="360"
	aunit="deg"/>


  <ellipsoid name="PMTActive1" lunit="cm" ax="10.0" by="10.0" cz="8.0" zcut1="4.0"/>
  <ellipsoid name="PMTActive2" lunit="cm" ax="9.9" by="9.9" cz="7.9" zcut1="4.0"/>
  <subtraction name="PMTActiveArea">
    <first ref="PMTActive1"/>
    <second ref="PMTActive2"/>
    <position name="PMTActiveShift" unit="cm" z="-.1"/>
    <rotationref ref="identity"/>
  </subtraction>
  <ellipsoid name="PMTGlassInactive1" lunit="cm" ax="10.0" by="10.0" cz="8.0" zcut2="4.0"/>
  <ellipsoid name="PMTGlassInactive2" lunit="cm" ax="9.9" by="9.9" cz="7.9" zcut2="4.0"/>
  <subtraction name="PMTGlassInactive">
    <first ref="PMTGlassInactive1"/>
    <second ref="PMTGlassInactive2"/>
    <position name="PMTGlassInactiveShift" unit="cm" z=".1"/>
    <rotationref ref="identity"/>
  </subtraction>
  <tube name="PMTLowerGlass" lunit="cm" rmin="0.0" rmax="4.225" z="9.0" deltaphi="360.0" aunit="deg"/>
  <tube name="PMTBase" lunit="cm" rmin="0.0" rmax="2.56" z="3.0" deltaphi="360.0" aunit="deg"/>

  <ellipsoid name="PMTVacuum" lunit="cm" ax="9.9" by="9.9" cz="7.9"/>

</solids>

<structure>

  <volume name="volTarget">
    <materialref ref="scintCocktailLAB"/>
    <solidref ref="scintillatorTarget"/>
  </volume>

  <volume name="volInnerTank">
    <materialref ref="acrylic_uvt_good"/>
    <solidref ref="innerTank"/>
    <physvol name="pvTarget">
      <volumeref ref="volTarget"/>
      <position name="posTargetVolume" unit="m" x="0" y="0" z="0"/>
    </physvol>
  </volume>

  <volume name="volPMTActiveArea">
    <materialref ref="scintCocktailLAB"/>
    <solidref ref="PMTActiveArea"/>
  </volume>

  <volume name="volPMTGlassInactive">
    <materialref ref="borosilicate_glass"/>
    <solidref ref="PMTGlassInactive"/>
  </volume>

  <volume name="volPMTVacuum">
    <materialref ref="Vacuum"/>
    <solidref ref="PMTVacuum"/>
  </volume>

  <volume name="volPMTLowerGlass">
    <materialref ref="borosilicate_glass"/>
    <solidref ref="PMTLowerGlass"/>
  </volume>

  <volume name="volPMTBase">
    <materialref ref="stainless_steel"/>
    <solidref ref="PMTBase"/>
  </volume>
"""
#  <volume name="volPMTVacuum">
#    <materialref ref="Vacuum"/>
#    <solidref ref="PMTVacuum"/>
#  </volume>
#
#  <volume name="volPMTGlassActive">
#    <materialref ref="scintCocktailLAB"/>
#    <solidref ref="PMTGlassActive"/>
#  </volume>
#
#  <volume name="volPMTGlassInactive">
#    <materialref ref="borosilicate_glass"/>
#    <solidref ref="PMTGlassInactive"/>
#  </volume>
#  
#  <volume name="volPMTBase">
#    <materialref ref="stainless_steel"/>
#    <solidref ref="PMTBase"/>
#  </volume> 
#
#"""

#  <ellipsoid name="PMTGlassActive" lunit="cm" ax="10.0" by="10.0" cz="8.0" zcut1="4.0"/>
#  <ellipsoid name="PMTGlassInactive" lunit="cm" ax="10.0" by="10.0" cz="8.0" zcut2="4.0"/>
#  <ellipsoid name="GlassBound" lunit="cm" ax="10.1" by="10.1" cz="8.1"/>
#  <tube name="PMTBase" lunit="cm" rmin="0.0" rmax="4.225" z="6.0" deltaphi="360.0" aunit="deg"/>
#  <tube name="BaseBound" lunit="cm" rmin="0.0" rmax="4.325" z="6.2" deltaphi="360.0" aunit="deg"/>
#  <union name="PMTBound">
#    <first ref="GlassBound"/>
#    <second ref="BaseBound"/>
#    <positionref ref="PMTBaseOffset"/>
#    <rotationref ref="identity"/>
#  </union>
#
#  <ellipsoid name="PMTVacuum" lunit="cm" ax="9.7" by="9.7" cz="7.7"/>  
 
#  <volume name="volPhotoCathode">
#    <!-- <materialref ref="scintillator"/> -->
#    <materialref ref="photocathode"/>
#    <solidref ref="pctube"/>
#  </volume>
#  <volume name="volPMTShield">
#    <materialref ref="aluminum"/>
#    <solidref ref="pmt_shield"/>
#  </volume>
#  <volume name="volPMTGlassTube">
#    <materialref ref="borosilicate_glass"/>
#    <solidref ref="pmt_glasstube"/>
#    <physvol>
#      <volumeref ref="volPMTShield"/>
#      <position name="posPMTShield" unit="cm" x="0" y="0" z="0.5*22.0-0.5*11.0"/>
#    </physvol>
#  </volume>
#  <volume name="volActivePMT">
#    <materialref ref="scintCocktailLAB"/>
#    <solidref ref="pmt_bound"/>
#    <physvol>
#      <volumeref ref="volPMTGlassTube"/>
#      <positionref ref="origin"/>
#    </physvol>
#    <physvol>
#      <volumeref ref="volPhotoCathode"/>
#      <positionref ref="photocathode_offset"/>
#    </physvol>
#  </volume>
#  <tube name="pmt_bound" lunit="cm" aunit="deg" rmin="0" rmax="11.0" z="24.0" startphi="0" deltaphi="360.0"/>
#  <tube name="pctube" lunit="cm" aunit="deg" rmin="0" rmax="9.4" z="0.2" startphi="0" deltaphi="360.0"/>
#  <tube name="pmt_outertube" lunit="cm" aunit="deg" rmin="0" rmax="10.20" z="22.0" startphi="0" deltaphi="360.0"/>
#  <tube name="pmt_shield" lunit="cm" aunit="deg" rmin="9.4" rmax="10.0" z="11.0" startphi="0" deltaphi="360.0"/>
#  <subtraction name="pmt_glasstube">
#    <first ref="pmt_outertube"/>
#    <second ref="pctube"/>
#    <positionref ref="photocathode_offset"/>
#    <rotationref ref="identity"/>
#  </subtraction>
#
#"""

#  <!-- building the world inside-out -->
#  <volume name="volActivePMT">
#    <materialref ref="scintCocktailLAB"/>
#    <solidref ref="pmt_active"/>
#  </volume>
#"""

part2 = """ 
  <volume name="volOuterTank">
    <materialref ref="stainless_steel"/>
    <solidref ref="outerTank"/>
    <physvol name="pvOuterScintillator">
      <volumeref ref="volOuterScintillator"/>
      <position name="posOuterScintillator" unit="m" x="0" y="0" z="0"/>
    </physvol>
  </volume>
"""

part3 = """

  <volume name="volWorld">
    <materialref ref="air"/>
    <solidref ref="world"/>
    <physvol name="pvOuterTank">
      <volumeref ref="volOuterTank"/>
      <position name="posTank" unit="m" x="0" y="0" z="0"/>
    </physvol>
  </volume>

</structure>

<setup name="Default" version="1.0">
  <world ref="volWorld" />
</setup>

</gdml>
"""
