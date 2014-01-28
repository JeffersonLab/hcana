Installation of Eclipse (Kepler) on MacOSX, with plugins for SCons, Git, GitHub, and Python
===========================================================================================

Install Eclipse
---------------

There exists an Eclipse plugin for SCons, called SConsolidator.  As of
January, 2014, this plugin works only with the Kepler version of
Eclipse.  To download and install this version of Eclipse, go to:

[Eclipse Kepler SR1 Downloads](http://www.eclipse.org/downloads/packages/eclipse-ide-cc-developers/keplersr1)

and download the MacOSX (Cocoa 64) version.  The file should be called:

eclipse-cpp-kepler-SR1-macosx-cocoa-x86_64.tar.gz

Unpack this somewhere in your own user space.  I chose to install it
directly underneath my home directory, but of course you can install
it where you like.

	$ cd
	$ sudo tar -xvzf ~/Downloads/eclipse-cpp-kepler-SR1-macosx-cocoa-x86_64.tar.gz

Start up Eclipse from the command line with:

	$ ~/eclipse/eclipse

You might also find it useful to add the Eclipse app to your dock,
permanently, to make it easier to start up in the future.

When you start up Eclipse, you will get a window asking you to choose
a workspace.  The default is usually /Users/****/Documents/workspace.
This is fine, but you might want to choose someplace else.  Make sure
to click the 'Use this as the default ...' radio button so that you
will not get asked this every time.

When Eclipse finally starts up, you can click on the "X" on the
welcome page, and it should take you to the Project Explorer page.
This is the main view that you will probably end up using most of the
time.

Install the SConsolidator Plugin
--------------------------------

In order for Eclipse to work with SCons, one has to install a plugin
called SConsolidator.  To do this, do the following:

a) Go to Help->Install New Software

b) In the Install window that comes up, in the "Work with" field, enter:

http://www.sconsolidator.com/update

and click on "Add..."

c) Specify the name as "SConsolidator"

d) Check the Eclipse Plug-In for SCons radio button, then hit Next>
and follow the installation through.

e) After the installation, Eclipse will need to be restarted, which it
should do automatically.

When Eclipse restarts, it will ask you about setting the path to
SCons.  Set this up now.  Check to see where SCons is installed by
doing "which scons" from a terminal window.  For me, it is in
/usr/local/bin/scons, and so I entered that for the path to the SCons
executable.

To change the SCons build options (at any time), you can go to
Eclipse->Preferences, and then expand the SCons tag, to reveal options
for Build Settings, Performance vs. Accuracy, and Warnings.  In
particular, if you want to build the standalone executables, you can
add 'standalone=1' to the SCons Options in Build Settings.

Install PyDev for Python Support
--------------------------------

PyDev includes a nice Python editor with appropriate syntax
highlighting, as well as providing the "mouse-over" documentation
features of Eclipse for those things that are written in Python -
namely SCons.

As of this writing, Eclipse Kepler does not play so nicely with
PyDev-3.X, and so we will install PyDev-2.X instead.  The reason may
be related to issues with Java 1.6 vs. 1.7, in fact.

a) Go to Help->Install New Software

b) In the Install window that comes up, in the "Work with" field, enter:

http://pydev.org/updates

and click on "Add..."

c) Specify the name as "PyDev" and hit return

d) IMPORTANT: Uncheck the radio button to show versions other than the
latest version in the bottom portion of this window.

e) Look for the PyDev for Eclipse Version 2.8.2, and check this radio button.

d) Proceed with the installation.

e) IMPORTANT: At some point it may ask you to allow a certain security
certificate - you actually have to check the radio button in the top
part of the window and THEN accept the certificate.

f) After the installation, Eclipse will need to be restarted, which it
should do automatically.

Restart Eclipse; you should now have access to PyDev. You should see
it by going to Eclipse->About Eclipse->Installation Details->Installed
Software

Setting up Git within Eclipse
-----------------------------

Much of this comes from [this article](http://www.vogella.com/tutorials/EclipseGit/article.html).

a)  Basic Configuration

Select Window → Preferences → Team → Git

Under the field for Cloning Repositories, choose a location (locally) for storing respositories that
you will eventually clone to create.

Select Window → Preferences → Team → Git → Configuration

Choose the User Settings Tab:

If you have already been using git on your system, you should see that the user and email fields 
are already filled out.  If they are now, you can fill them out now.  The 'location' field should
be ($HOME)/.gitconfig, where ($HOME) is your normal home directory

b)  Git Staging View

Select Window → Show View → Other... → Git → Git Staging

You should now see a Git Staging tab in the Console area of the main Eclipse window.  Basically, this
window will give you all information that would be reported by 'git status' at the command line.

c) Activate the Git Toolbar

Select Window → Customize perspective... and check the command groups Git and Git Navigation Actions
in the Command Groups Availability tab.  You should now see "Git" as one of the main pulldown menus
of the Eclipse toolbar.
 
Opening hcana as a New Project from Exisiting Source within Eclipse
------------------------------------------------------------------

This section assumes that you have already cloned hcana (from the command line) somewhere on your system.

a) Choose File->New->Other

b) In the pop-up window, choose 'New SCons project from existing source', and click 'Next'

c) Choose a project name (hcana or hcana-1.6 are good choices), specify the existing code location, and
then add any SCons Options (for example, debug=1 standalone=1 will compile the code in debug
mode, and compile the standalone codes as well.

d) Click 'Finish'

You should now see the project within the Project Explorer window on the RHS of the Eclipse window.
You can double-click on any file, and it should open in the editor window.  You can edit and save
changes in the usual way.  Clicking on the "Hammer" icon on the Eclipse toolbar should execute
scons, with the output of the build process shown in the console view at the bottom of the Eclipse
window.

Also, you should see the current git branch that you are working on displayed next to the 
top-level hcana folder that is displayed in the Project Explorer window.

If you right-click on the hcana folder in the Project Explorer window, and then choose "Team", you
should see the various git commands available.  From here, you can switch to another branch,
checkout a new branch, do commits and pushes, and/or fetch and merge from upstream.  If you have
configured Eclipse to show the Git Toolbar, you can also choose a number of oft-used git commands
from the Git pulldown menu.

Also, you can commit and push from the Git Staging window!!!  And, you show the commit history
by right-clicking on the top level hcana folder, and choosing Team -> Show in History
You should see now a tab in the Console area called History, with a table of commits for the repository.

Cloning hcana from within Eclipse
--------------------------------

This section details how to clone your forked copy of hcana directly from within Eclipse (without
having done so at the command line already).

a) Select File → Import → Git → Project from Git.

b) Select Clone URI in the next dialog.

c) Enter the URI of your repository on github, for example:

git@github.com:brash99/hcana.git

d) Click on ‘Next’ to get the Branch Selection dialog

e) The default is to clone all branches.  You may want to select only certain ones.  Do so, 
if you please, and click on ‘Next’

f) Choose the local destination for your cloned repository.  Note that Eclipse will
warn you if there is already an existing repository clone there (which can happen as the 
default is to choose your top-level directory plus the first part of the repository name, 
i.e. hcana, and so this may already be used).

g) Choose the initial branch that you want to work with.

h) IMPORTANT!!!!  Click the radio button to Clone Submodules!!!!

i) Click on ‘Next’ to get the Projects dialog.

j) Click the ‘Use New Project wizard’ radio button, and then click on ‘Finish’

k)  The final dialog will allow you to choose ‘New SCons project from existing source’, at 
which point you can then continue from step b) in Section 5 above!! 

l) The final step is to add the appropriate Jefferson Lab reposistory as an upstream remote.

(i) Open the Git Repositories view (Select Window → Show View → Other... → Git → Git Repositories)

(ii) From the Git Repositories tab in the Console area, expand the repository that you are trying
to set up (hcana in this case), and then expand Remotes.  You should see that origin is present.

(iii) Right-click on Remotes, and choose 'Create Remote'.  Specify the Remote Name as 'upstream'. Select
'Configure Push', and Click OK.

(iv) Click on "Change" next to the field where one specifies the URI ... in the new dialog, specify
the remote URI as git@github.com:JeffersonLab/hcana.git.  Click on 'Save'.  You should now see that the
remote called upstream has been added.

(v)  The final step is to configure the upstream fetch so that it pulls from the develop branch.  Expand
the upstream directory under Remotes.  You will see a fetch and a push specification for the upstream
repository.  The fetch specification is indicated by a left green arrow.  Right-click on this, and 
choose Configure Fetch.  From the next dialog, choose Add.  In the next dialog, for the Source, specify
the develop branch, then click Save and Finish.
