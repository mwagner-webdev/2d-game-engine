namespace eval ::faw::lib::char {
	namespace export char

	namespace import ::faw::core::*
}

proc ::faw::lib::char::char {type name layer} {
	set path files/img/chars/
	set sep _
	set dirs [list n s w e]
	set anims [list m l m r]
	set ext .png

	foreach dir $dirs {
		foreach anim $anims {
			lappend $dir $path$name$sep$dir$anim$ext
		}
	}

	return [$type $n $s $w $e $layer]
}
