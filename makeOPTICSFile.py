import math

#Properties
LIGHT_YIELD = 1000.

scintWavelengths = [360, 365, 370, 380, 390, 400, 405, 410, 415, 420, 425, 430, 435, 440, 445, 450, 455, 460, 470, 480, 490, 500, 510, 520, 530, 540, 800]
scintValues = [0.0, 0.0, 0.0, 0.02, 0.1, 0.5, 0.53, 0.5, 0.45, 0.55, 0.65, 0.6, 0.5, 0.4, 0.35, 0.32, 0.31, 0.3, 0.2, 0.13, 0.1, 0.05, 0.03, 0.02, 0.01, 0.0, 0.0]

scintIntegral = 0.0

for i in range(0, len(scintWavelengths) - 1):
	scintIntegral += (scintWavelengths[i+1] - scintWavelengths[i])*scintValues[i]

#Have to correct for the fact that for protons RAT chooses to use the scintillation integral
#and ignores that LIGHT_YIELD parameter for some reason. This normalizes the integral.
correctionFactor = LIGHT_YIELD/scintIntegral

for i in range(0, len(scintValues)):
	scintValues[i] *= correctionFactor

# Empirical values for Sellmeier formula taken from:
# https://arxiv.org/pdf/1105.2101.pdf
# Ellipsometric measurements of the refractive indiced of linear alkylbenzene and
# EJ-301 scintillators from 210 to 1000 nm

bVals = [.821384, .311375, .0170099, .608268]
cVals = [94.7625, 160.751, 219.575, 9385.54]

outfile = open('OPTICS_JSNS2.ratdb', 'w')
outfile.write("""
{
name: "OPTICS",
index: "scintCocktailLAB",
valid_begin: [0, 0],
valid_end: [0, 0], \n
""")

outfile.write("LIGHT_YIELD: " + str(LIGHT_YIELD) + ',\n')
outfile.write("LIGHT_YIELDneutron: " + str(LIGHT_YIELD) + ',\n')

# Absorption measurements taken from:

# http://iopscience.iop.org/article/10.1088/1674-1137/34/11/011/pdf
# Study of absorption and re-emission processes in a ternary liquid scintillation system

outfile.write("""
ABSLENGTH_option: "wavelength",
ABSLENGTH_value1: [350d, 360d, 370d, 380d, 390d, 400d, 410d, 420d, 430d, 800d],
ABSLENGTH_value2: [1.0d-1, 2.0d-1, 2.5d-1, 3.0d-1, 1.0d, 7.0d, 2.0d2, 2.0d3, 4.0d3, 8.0d3],
""")

# Re-emission probabilities taken from:

# https://arxiv.org/pdf/1506.00240.pdf
# Measurement of the fluorescence quantum yield of bis-MSB

outfile.write("""
REEMISSION_PROB_option: "wavelength",
REEMISSION_PROB_value1: [355d, 365d, 375d, 385d, 390d, 395d, 400d, 405d, 410d, 415d, 420d, 425d, 430d, 450d, 800d],
REEMISSION_PROB_value2: [.92d, .9d, .85d, .92d, .92d, .85d, .8d, .65d, .7d, .6d, .45d, .25d, .05d, 0.0d, 0.0d],
""")

# Scintillation waveform properties taken from Table VII of:

# https://arxiv.org/pdf/1104.5620.pdf
# The next-generation liquid-scintillator neutrino observatory LENA

outfile.write("""
SCINTWAVEFORM_value1: [-4.6d, -18d, -156d],
SCINTWAVEFORM_value2: [.71d, .22d, .07d],
""")

# Emission spectrum measurements taken from:

# http://iopscience.iop.org/article/10.1088/1674-1137/34/11/011/pdf
# Study of absorption and re-emission processes in a ternary liquid scintillation system

outfile.write("""
SCINTILLATION_option: "dy_dwavelength",
SCINTILLATION_value1: [""")
for scintWavelength in scintWavelengths:
	outfile.write(str(scintWavelength) + "d, ")
outfile.write("""],
SCINTILLATION_value2: [""")
for scintVal in scintValues:
	outfile.write(str( round(scintVal, 2) ) + "d, ")
outfile.write("],\n\n")

outfile.write("""
RINDEX_option: "wavelength",
RINDEX_value1: [""")

waveLenList = [300, 350, 400, 450, 500, 550, 600, 650, 700, 750, 800]

for wavelength in waveLenList:
	outfile.write(str(wavelength) + "d, ")
outfile.write("],")

rValList = []
for wavelength in waveLenList:
	rVal = 1.0
	for i in range(0, len(bVals)):
		rVal += bVals[i] / (1.0 - (cVals[i] / wavelength)**2.0)
	rValList.append(math.sqrt(rVal))

outfile.write("\nRINDEX_value2: [")
for rVal in rValList:
	outfile.write(str( round(rVal, 2) ) + "d, ")
outfile.write("],\n\n")

	

outfile.write('PROPERTY_LIST: ["LIGHT_YIELD", "ABSLENGTH", "REEMISSION_PROB", "SCINTWAVEFORM", "SCINTILLATION", "RINDEX"]\n')

outfile.write("}")

outfile.close()
