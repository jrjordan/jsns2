
{
name: "GEO",
valid_begin: [0, 0],
valid_end: [0, 0],
gdml_file: "jsns2new.gdml",
opdet_lv_name: "volPMTActiveArea",
}

{
name: "GEO",
index: "border_veto_outer",
valid_begin: [0, 0],
valid_end: [0, 0],
mother:"border",
type:"border",
volume1:"pvOuterScintillator",
volume2:"pvOuterTank",
surface: "stainless_steel",
reverse: 1,
}
