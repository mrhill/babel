#!/usr/bin/perl
# Extract blocks from Blocks.txt

$ver = <>; chop $ver;
($sec,$minu,$hour,$mday,$mon,$year,$wday,$yday,$isdst)=localtime(time);
printf "/* Generated from %s on %4d-%02d-%02d %02d:%02d:%02d */\n\n",$ver, $year+1900,$mon+1,$mday,$hour,$minu,$sec;

@a_start  = ();
@a_ct     = ();
@a_name   = ();
@a_define = ();

$namemaxlen = 0;
$ctcont=0;
$ctBMP=0;

# parse input
while(<>) {
  chop;
  if ((length($_) != 0 ) && (substr($_, 0, 1) ne '#')) {
    ($range,$name) = split(/; /, $_);
    ($start,$end)  = split(/\.\./, $range);
    $start = hex $start;
    $end   = hex $end;
    $def = uc $name;
    $def =~ s/[ ,-]/_/g;
    $def = '#define bbCPGUNICODE_BLOCK_'.$def;

    if ($#a_ct >= 0) {
      $laststart = $a_start[$#a_ct];
      $lastend   = $laststart + $a_ct[$#a_ct];
      if  ($lastend != $start) {
        $ctcont++;
        push @a_start, $lastend;
        push @a_ct, $start-$lastend;
        push @a_name, '';
        push @a_define, '';
      }
    }

    push @a_start, $start;
    push @a_ct, $end-$start+1;
    push @a_name, $name;
    push @a_define, $def;

    if (length($name) > $namemaxlen) { $namemaxlen=length($name); }
  }
}

$ct=$#a_ct+1;

for($i=0; $i<$ct; $i++) {
  if ($a_name[$i] ne '') {
    printf "$a_define[$i] $i\n";
  }
}
print "#define bbCPGUNICODE_BLOCKCOUNT $ct\n\n";

print "#define bbCPGUNICODE_BLOCKNAMES \\\n";
for($i=0; $i<$ct; $i++) {
  if ($a_name[$i] ne '') {
    print 'bbT("'.$a_name[$i].'")';
  } else {
    print 'NULL';
  }
  if ($i<$#a_ct) {
    print ",\\\n";
  } else {
    print "\n";
      }
}
print "#define bbCPGUNICODE_BLOCKNAME_MAXLEN $namemaxlen\n\n";

print "#define bbCPGUNICODE_BLOCKINDEX \\\n";
for($i=0; $i<$ct; $i++) {
  if ($a_start[$i] < 0x100000) {
    if ($a_start[$i] & 15) {
      die "ERROR, range start must be multiple of 16\n";
    }
    if ($i) {
      print ',';
      if (($i & 7)==0) { print "\\\n"; }
    }
    printf '0x%04XU', $a_start[$i] >> 4;
  } else {
    $j=$i;
    $i=$ct;
    print "\n";
  }
}

for ($i=$j-1;$i<$ct; $i++) {
  printf "#define bbCPGUNICODE_BLOCKINDEX_OOB%d 0x%06XUL\n", $i-$j+1, $a_start[$i]+$a_ct[$i];
}
printf "#define bbCPGUNICODE_BLOCKINDEX_OOBCOUNT %d\n\n", $ct-$j+1;

print "#define bbCPGUNICODE_BLOCKMASK ";
$m=0; $s=',';
for($i=0; ($i<$ct)&&($s eq ','); $i++) {
  if ($a_start[$i] < 0x100000) {
    if ($a_name[$i] ne '') {
      $m |= 1 << ($i & 31);
    }
  } else {
    $s="\n";
  }
  if ((($i & 31) == 31) || ($a_start[$i] >= 0x100000)) {
    printf "0x%08XUL$s", $m;
    $m=0;
  }
}
