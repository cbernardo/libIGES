24 May 2015
Updates:
  + The code is now capable of creating bare PCB models
  + The "mergetest" test program can now create assemblies
    with Manifold Solid B-Rep Objects which means output
    from more MCAD systems (including Pro-E) are now
    supported.
  + In principle the code now has all features required
    to create a PCB assembly with a correct PCB model
    and component models as provided by various sources.

  Next step: Verify PCB model generation by converting
  IDFv2 or IDFv3 files to an IGES assembly.

6 May 2015

Update: The code can now read a variety of IGES files and create
assemblies. Some files such as various Molex connector models
cannot be read due to unimplemented entities; such files will
typically make use of manifold B-Rep entities. Support will
eventually be added for these but at the moment the tools needed
to create a solid model of the bare PCB have the highest priority.

The outline manipulation code (addition/subtraction of closed
loops consisting of a circle or lines and/or arcs) is finished
and appears to work. There may be some corner cases which have
not been tested so this code is not guaranteed to be bug-free.
See the output of the "olntest" test program to see the results
of the boundary manipulation code. At the moment the code can
generate the vertical sides of the PCB; the code to generate
the top and bottom planes of the PCB are next on the agenda.

8 Feb 2015

This is a work in progress; the aim is to create an implementation
of the IGESv6 specification which is suitable for use in the KiCad
project to create a board model and assembly model.

