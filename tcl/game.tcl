source tcl/lib/textbox.tcl
source tcl/lib/char.tcl

namespace import ::faw::core::*
namespace import ::faw::lib::char::*

# morning
#tint 192 216 255 32 300 300 300

# night
#tint 0 0 96 64 216 216 216

# desert
#tint 128 64 32 64 320 255 255

set map [map files/img/grass.png 100 100]

set ground [layer $map]
set grass [layer $map]
set lower [layer $map]
set mid [layer $map]
set upper [layer $map]

set piclayer [layer]

::faw::lib::textbox::init $piclayer

sound files/sound/music/test.mid

set player [char player zeppon $mid]

set mensch [char sprite be17 $mid]
obstruct $mensch 20 4 -4 -4
x $mensch 400
y $mensch 400
text $mensch ! 10 -12

on $mensch activate {
	namespace import ::faw::lib::textbox::*
	
	textbox "Hoi, ich bin so ein NPC der nur hier zum Test rumsteht. Meinen Text braucht man nicht mehr manuell umbrechen, die Engine ist so schlau und macht das für mich."
	textbox "Zweite Textbox."
	textbox "Blahblubb"
}

for {set i 0} {$i < 20} {incr i} {
	set dirt [dragsprite files/img/dirt.png $ground]
	x $dirt 0

	set darkdirt [dragsprite files/img/darkdirt.png $ground]
	x $darkdirt 48

	set highgrass [dragsprite files/img/highgrass.png $lower]
	x $highgrass 96

	set palisade [dragsprite files/img/palisade.png $mid]
	x $palisade 200
	obstruct $palisade  80 0 0 0

	set palisade_vert [dragsprite files/img/palisade_vert.png $mid]
	x $palisade_vert 200
	obstruct $palisade_vert 80 4 0 -4
}

set x 0
set y 200

for {set i 0} {$i < 200} {incr i} {
	incr x [expr int(rand() * 500) + 100]
	incr y [expr int(rand() * 200) - 100]

	if {$x >= 2400} {
		set x 0
		incr y [expr int(rand() * 500) + 100]
	}
	
	set root [sprite files/img/root.png $lower]
	set tree [dragsprite files/img/tree.png $mid]

	obstruct $root

	follow $root $tree

	x $root 14
	y $root 107

	x $tree $x
	y $tree $y
}

set x 0
set y 200

for {set i 0} {$i < 200} {incr i} {
	incr x [expr int(rand() * 500) + 100]
	incr y [expr int(rand() * 200) - 100]

	if {$x >= 2400} {
		set x 0
		incr y [expr int(rand() * 500) + 100]
	}

	set root [sprite files/img/smallroot.png $lower]
	set tree [dragsprite files/img/smalltree.png $mid]
	
	follow $root $tree

	obstruct $root

	x $root 10
	y $root 88

	x $tree $x
	y $tree $y
}

set x 0
set y 200

for {set i 0} {$i < 200} {incr i} {
	incr x [expr int(rand() * 500) + 100]
	incr y [expr int(rand() * 200) - 100]

	if {$x >= 2400} {
		set x 0
		incr y [expr int(rand() * 500) + 100]
	}

	set root [sprite files/img/yatreeroot.png $lower]
	set tree [dragsprite files/img/yatree.png $mid]
	
	follow $root $tree

	obstruct $root

	x $root 35
	y $root 75

	x $tree $x
	y $tree $y
}

set x 0
set y 200

for {set i 0} {$i < 200} {incr i} {
	incr x [expr int(rand() * 500) + 100]
	incr y [expr int(rand() * 200) - 100]

	if {$x >= 2400} {
		set x 0
		incr y [expr int(rand() * 500) + 100]
	}

	set root [sprite files/img/bushroot.png $lower]
	set bush [dragsprite files/img/bush.png $mid]
	
	follow $root $bush

	obstruct $root

	x $root 24
	y $root 38

	x $bush $x
	y $bush $y
}
