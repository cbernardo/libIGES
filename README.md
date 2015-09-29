29 Sep 2015
  + Boost:
    All Boost dependencies have been removed.
  + Notes:
    If the library is configured with -DUSE_SISL=ON, a number
    of test programs will fail until IGES_ENTITY_126::GetStartPoint()
    and IGES_ENTITY_126::GetEndPoint() have been implemented.

28 Sep 2015
  + CHANGE OF LICENSE:
    As sole author of libIGES I am changing the license from
    GPLv3+ to LGPLv2.1+ to offer greater freedom to the users
    of this library.  Note that the SINTEF SISL library uses
    the GNU Affero GPL Version 3 and that its terms apply to
    libIGES if libIGES is compiled with SISL support.
    
  + Use of SISL:
    SISL has been removed from the base IGES objects to allow
    users to take advantage of the LGPLv2.1+ license of libIGES.
    The INSTALL instructions have been updated to show build
    configurations with and without SISL.
    
30 May 2015
  + The idf2igs tool now exports IGES assemblies which
    include the PCB and component models. If your MCAD
    supports the color attribute of IGES then your
    model will be displayed in glorious color.

29 May 2015
  + Added the "idf2igs" tool (work in progress) based on
    the IDFv3 framework from the KiCad project.
    This tool is based on the idf2vrml source and as
    such currently contains a lot of cruft. At the moment
    bare PCBs with arbitrary boundaries and cutouts can be
    generated. Overlapping drill holes are handled correctly
    so that IDF files from ECADs such as Eagle, which
    represent slots using multiple overlapping drills,
    can be modeled.

    Intentional limitations to keep in mind when creating
    test IDF files:

    a. ensure that no drill holes or cutouts are completely
    outside the main outline and that no drill holes are
    completely inside another drill hole or cutout.
    Unfortunately ECAD software does not typically enforce
    this constraint so it is possible to generate such
    bad geometry.

    b. ensure that no zero-thickness walls are created,
    for example a drill hole tangent to a board edge or
    two tangent drill holes.

    c. Ensure that cutouts and drill holes intersect the
    main outline at no more than 2 points; this is to
    guarantee that the board will not be split into two
    separate bodies.

    The code has been generated on a number of test shapes
    and on the IDF models of the following boards:
    (a) pic_programmer demo from KiCad
    (b) video demo from KiCad
    (c) Arduino Due

    I intend for this tool to be refined to provide all
    the following features:
    a. A complete board assembly model in IGES
       (currently only a single bare PCB model is generated)
    b. embedded IGES models of all IDF component extrusions
    c. Colored PCB and components
    e. sensible naming of the parts (board model, component
       models, component instances)

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

