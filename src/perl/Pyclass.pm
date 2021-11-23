##############################################################
#
# Python wrapper class generation
#

package Pyclass;

use File::Basename;
use File::Path 'mkpath';

use strict;
use Utils;
use Parser;

our $out_dir;

##########

sub genWrapper($)
{
  my $cls = shift;

  my $qifname = $cls->{"qifname"};
  my $qif_fname = $cls->{"file"};


  my ($in_base, $in_dir, $in_ext) = fileparse($qif_fname, '\.qif');
  $in_dir =  "" if ($in_dir eq "./");
  my $out_fname = "$in_dir${in_base}.py";

  if ($out_dir) {
    $out_fname = "$out_dir/${in_base}.py";
    if (!-d $out_dir) {
        mkpath($out_dir) or die "Cannot create dir $out_dir: $!";
    }
  }

  print("Output Python file: $out_fname\n");

  open(OUT, ">$out_fname") || die "$?:$!";
  set_building_file($out_fname);

  my $py_clsname = $qifname;
  my $clsdb = $Parser::db{$qifname};

  print OUT "#\n";
  print OUT "# Python wrapper class for $qifname\n";
  print OUT "# autogenerated by mcwrapgen3.pl\n";
  print OUT "#\n";
  print OUT "\n";
  print OUT "import cuemol\n";
  print OUT "ci = cuemol.ci\n";

  my @extend_qif = @{$clsdb->{"extends"}} if ($clsdb->{"extends"});
  my @extends;

  foreach my $i (@extend_qif) {
    print OUT "from . import $i\n";
    push(@extends, "${i}.${i}")
  }

  if (!@extends) {
    # print OUT "from . import basewrapper\n";
    print OUT "from cuemol.wrapper_base import WrapperBase\n";
    @extends = ("WrapperBase");
  }

  my $extend_joined;
  $extend_joined = join(", ", @extends);


  print OUT "\n";
  print OUT "class ${py_clsname}($extend_joined):\n";
  print OUT "\n";
  print OUT "    def __init__(self, aWrapped):\n";
  print OUT "        super().__init__(aWrapped)\n";
  print OUT "\n";

  genCodeImpl($clsdb, $qifname);

  close(OUT);
}
		   
sub genCodeImpl($$)
{
  my ($clsdb, $class_name) = @_;

  # print OUT "##### From class $supcls_name\n";
  # print OUT "\n";

  # my $clskey = "\@implements_$supcls_name";
  # print OUT "#  ${class_name}[\"$clskey\"] = \"yes\";\n\n";

  genPropCode($clsdb, $class_name);

  genInvokeCode($clsdb, $class_name);
}

sub genPropCode($$)
{
  my $cls = shift;
  my $clsname = shift;

  return unless ($cls->{"properties"});

  my %props = %{$cls->{"properties"}};

  foreach my $propnm (sort keys %props) {

    my $prop = $props{$propnm};
    my $type = $prop->{"type"};
    # debug("JS: prop: $propnm, type: $type\n");

    print OUT "# property: $propnm, type: $type\n";
    print OUT "\n";

    if ($type eq "object") {
      genObjPropCode($clsname, $propnm, $prop);
    }
    elsif ($type eq "enum") {
      genEnumPropCode($clsname, $propnm, $prop);
    }
    else {
      genBasicPropCode($clsname, $propnm, $prop);
    }
  }
}

sub genBasicPropCode($$$)
{
  my $classnm = shift;
  my $propnm = shift;
  my $prop = shift;

  print OUT "    \@property\n";
  print OUT "    def $propnm(self):\n";
  print OUT "        return cuemol.createWrapper(ci.getProp(self._wrapped, \"$propnm\"))\n";
  print OUT "\n";

  return if (contains($prop->{"options"}, "readonly"));

  print OUT "    \@${propnm}.setter\n";
  print OUT "    def ${propnm}(self, aVal):\n";
  print OUT "        ci.setProp(self._wrapped, \"${propnm}\", aVal)\n";
  print OUT "\n";
}

sub genObjPropCode($$$)
{
  my $classnm = shift;
  my $propnm = shift;
  my $prop = shift;

  my $propqif = $prop->{"qif"};

  print OUT "    \@property\n";
  print OUT "    def $propnm(self):\n";
  print OUT "        return cuemol.createWrapper(ci.getProp(self._wrapped, \"$propnm\"))\n";
  print OUT "\n";

  return if (contains($prop->{"options"}, "readonly"));

  print OUT "    \@${propnm}.setter\n";
  print OUT "    def ${propnm}(self, aVal):\n";
  print OUT "        ci.setProp(self._wrapped, \"${propnm}\", aVal._wrapped)\n";
  print OUT "\n";
}

sub genEnumPropCode($$$)
{
  my ($classnm, $propnm, $prop) = @_;

  genBasicPropCode($classnm, $propnm, $prop);

  defined($prop->{"enumdef"}) || die;

  my %enums = %{ $prop->{"enumdef"} };
  foreach my $defnm (sort keys %enums) {
    my $key = $propnm."_".uc($defnm);
    my $value = $enums{$defnm};

    print OUT "    \@property\n";
    print OUT "    def $key(self):\n";
    print OUT "        return ci.getEnumDef(self._wrapped, \"$propnm\", \"$defnm\")\n";
    print OUT "\n";

    # print OUT "\n";
    # print OUT "${classnm}.prototype.__defineGetter__(\"$key\", function()\n";
    # print OUT "{\n";
    # print OUT "  return this._wrapped.getEnumDef(\"$propnm\", \"$defnm\");\n";
    # print OUT "});\n";
    # print OUT "\n";

  }	  
}

#####################

sub genInvokeCode($$)
{
  my $cls = shift;
  my $classnm = shift;

  return if (!$cls->{"methods"});

  my %mths = %{$cls->{"methods"}};

  foreach my $nm (sort keys %mths) {
    my $mth = $mths{$nm};
    my $nargs = int(@{$mth->{"args"}});

    my $rettype = $mth->{"rettype"};
    my $rval_typename = $rettype->{"type"};

    print OUT "# method: $nm\n";

    print OUT "    def ${nm}(".makeMthSignt($mth)."):\n";
    
    if ($rval_typename ne "void") {
      print OUT "        rval = ";
    }
    else {
      print OUT "        ";
    }
    
    print OUT "ci.invokeMethod(".makeMthArg($mth).")\n";
    
    if ($rval_typename eq "object") {
      my $rettype_qif = $rettype->{"qif"};
      print OUT "        return cuemol.createWrapper(rval)\n";
    }
    elsif ($rval_typename eq "void") {
      print OUT "        return\n";
    }
    # elsif ($rval_typename eq "enum") {
    # }
    else {
      # basic types
      # print OUT "        return rval\n";
      print OUT "        return cuemol.createWrapper(rval)\n";
    }
    print OUT "\n";
  }

  print OUT "\n";
}

sub makeMthSignt($)
{
  my $mth = shift;
  my $args = $mth->{"args"};

  my $ind = 0;
  my @rval = ("self");
  foreach my $arg (@{$args}) {
    push(@rval, "arg_$ind");
    ++$ind;
  }
  return join(", ", @rval);
}

sub isCallbackObj($)
{
  my $arg = shift;

  return 0 if ($arg->{"type"} ne "object");

  return 1 if ($arg->{"qif"} eq "LScrCallBack");
  return 0;
}

sub makeMthArg($)
{
  my $mth = shift;
  my $args = $mth->{"args"};
  my $name = $mth->{"name"};

  my @rval = ("self._wrapped", "\"$name\"");

  my $ind = 0;
  foreach my $arg (@{$args}) {
    my $arg_type = $arg->{"type"};
    if (isCallbackObj($arg)) {
      # function obj can be directly passed to the ci methods (it is converted to callback obj in the wrapper)
      push(@rval, "arg_$ind");
    }
    elsif ($arg_type eq "object") {
      push(@rval, "arg_${ind}._wrapped");
    }
    elsif ($arg_type eq "dict") {
      push(@rval, "cuemol.conv_dict_arg(arg_${ind})");
    }
    else {
      push(@rval, "arg_$ind");
    }
    ++$ind;
  }
  return join(", ", @rval);
}

1;


