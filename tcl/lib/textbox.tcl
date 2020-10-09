namespace eval ::faw::lib::textbox {
	namespace export textbox

	namespace import ::faw::core::*

	proc init {layer} {
		variable tblayer $layer

		variable textbox [sprite files/img/textbox.png $tblayer]
		alpha $textbox 0
	}

	variable textboxes [list]

	proc pop {} {
		variable textboxes
		set textboxes [lrange $textboxes 0 end-1]
	}

	proc tbdisplay {} {
		variable textbox
		variable textboxes
		
		if {[llength $textboxes] > 0} {
			text $textbox [lindex $textboxes end] 64 264
			fade $textbox 255 10
			move $textbox 0 0 4

			on contpress {
				namespace import ::faw::core::*
				
				if {$contpress == 32} {
					sound files/sound/Kill2.wav
					::faw::lib::textbox::pop
					unbind contpress
				}
			}
		} else {
			fade $textbox 0 10
			move $textbox 0 128 4
		}
	}

	on frame {
		::faw::lib::textbox::tbdisplay
	}
}

proc ::faw::lib::textbox::textbox {text} {
	variable textboxes

	set textboxes [linsert $textboxes 0 $text]
}
