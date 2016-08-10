
{
name: "GEO",
valid_begin: [0, 0],
valid_end: [0, 0],
gdml_file: "jsns2.gdml",
opdet_lv_name: "volPMTActiveArea",
}

{
name: "GEO",
index: "border_veto_inner",
valid_begin: [0, 0],
valid_end: [0, 0],
mother:"border",
type:"border",
volume1:"pvBlackSheet",
volume2:"pvVetoScintillator",
surface: "ptfe_fabric",
reverse: 1,
}

{
name: "GEO",
index: "border_veto_outer",
valid_begin: [0, 0],
valid_end: [0, 0],
mother:"border",
type:"border",
volume1:"pvVetoScintillator",
volume2:"pvOuterTank",
surface: "ptfe_fabric",
reverse: 1,
}
