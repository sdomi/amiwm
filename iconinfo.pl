#!/usr//bin/env perl

sub chompstring {
    my ($len, $rest)=unpack("Na*", $_[0]);
    return unpack("a".$len."a*", $rest);
}

sub imginfo {
    my ($left, $top, $width, $height, $depth, $pp, $poo, $rest) =
	unpack("n5x4CCx4a*", $_[0]);

    print "Image: ${width}x${height}+$left+$top $depth bit\n";

    my $bpr=2*(($width+15)>>4);
    my $imgsz=$bpr*$height*$depth;

    return substr($rest, $imgsz);
}


%itypes=( 1=>disk, 2=>drawer, 3=>tool, 4=>project, 5=>garbage, 6=>device,
	  7=>kick, 8=>appicon );
undef $/; $i=<>;

($magic, $version, $gad, $ty, $deftool, $tts, $x, $y, $drwdata, $toolwin, $stacksize, $rest) =
    unpack("nna44CxNNNNNNNa*", $i);

($left, $top, $width, $height, $flags, $activ, $gty, $gid, $gud) =
    unpack("x4n7x20nN", $gad);

if($x==0x80000000) { $x="no position"; }
if($y==0x80000000) { $y="no position"; }

printf "Magic: %04x  Version: %d  Revision: %d  Type: $itypes{$ty}\n", $magic, $version, $gud&0xff;
print "X=$x, Y=$y, stacksize=$stacksize\n";
printf "Gadget: ${width}x${height}+$left+$top flags=%04x, activ=%04x\n", $flags, $activ;

if($drwdata!=0) {
    ($left, $top, $width, $height, $panx, $pany, $rest) =
	unpack("n4x40N2a*", $rest);
    printf "Window: ${width}x${height}+$left+$top(+$panx+$pany)\n";
}

$rest=imginfo($rest);
if($flags&2) { $rest=imginfo($rest); }

if($deftool!=0) {
    ($deftool, $rest)=chompstring($rest);
    printf "Default tool: $deftool\n";
}
if($tts) {
    ($arraybytes, $rest)=unpack("Na*", $rest);
    for(2..$arraybytes>>2) {
	($tooltype, $rest)=chompstring($rest);
	print "ToolType: $tooltype\n";
    }
}
if($toolwin) {
    ($toolwin, $rest)=chompstring($rest);
    print "ToolWin: $toolwin\n";
}
if($drwdata!=0 and $version>0) {
    ($wflags, $vmodes, $rest)=unpack("Nna*", $rest);
    printf "Window: flags=%08x viewmodes=%04x\n",$wflags,$vmodes;
}
printf "%d bytes remaining.\n", length($rest);
