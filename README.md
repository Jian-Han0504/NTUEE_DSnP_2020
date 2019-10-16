# comGit
For my computer works!
Install Git on Ubuntu 18.04
1. sudo apt-get install git
2. GUI: sudo apt-get install gitk
3. set envionment:
   $ git config --global user.name "<user_name>"
   $ git config --global user.email "<user_email>"
   // My github account is Jian-Han0504 jh880507@gmail.com
4. make directory: (use mkdir command)
   $ cd ~/Desktop/<git_name> // in my computer, I use "myGit"
   $ git init // outputs: Initialized empty Git repository in /home/jian-han/Desktop/myGit/.git/
   // ****** use "ls -al" to check .git file <- "controller"

5. common commands:
   $ git status // check the status under the folder
   $ echo "<context>" > <filename> // add context to the new file <filename>
   $ touch <file> // create a new file
   $ git add <filename> // git will track the file
			// use git add --all to add all files or git add *.<type> to add all files with the same file type

   $ git commit -m "<todo>" // to commit the file. Put what u do in <todo>.
			    // only commit those in staging area
   $ git commit -a -m "update content" // to combine add and commit steps, useless for untracked files
   $ git log // to check records
	     // different style for git log --oneline --graph
	     // git log --oneline --author="Sherly" to check records made by Sherly. Use | to search multiple authors
	     // git log --oneline --grep="<context>" to check records with the particular context
	     // git log --oneline --since="9am" --until="12am" --after="2017-01"

   remove files
   $ rm <filename> // remove is an untracked behaviour, need add and commit
   $ git rm <filename> // remove file, need not to use add command again
   $ git rm <filename> --cached // the file won't be tracked by Git, but still exists

   rename
   $ mv <prv_filename> <alt_filename> // again, this action is still untracked! Use "git mv" to combine steps!

6. Remote GitHub
   a. Sign in to Github
   b. Create a new repository // comGit in practice
   c. set key:
	$ ssh-keygen -t rsa -C "email_address"
	Store ssh key "id_rsa" and "id_rsa.pub" under ~/.shh (default)
	Create a passphrase (ex: jhAPTX4869 in my laptop)
	
	$ ssh-agent // activate
	$ ssh-add <file_loc> // in practice: ssh-add ~/.shh/id_rsa
	$ subl ~/.ssh/id_rsa.pub // use Sublime Text to see public key, then copy it in Sublime Text
	
	Open GitHub, go to the setting part and click "New SSH Key", and then paste context within "id_rsa.pub" to the blank (also name a title)
	If pass,
	$ ssh -T git@github.com
	// to authenticate
   d. Under myGit directory, type 
      $ git remote add origin git@github.com:Jian-Han0504/comGit.git // "origin": name of the remote node, can be altered
      $ git push -u origin master
      // there might be many kinds of problems: refer to "https://help.github.com/en/articles/dealing-with-non-fast-forward-errors"
      // and "https://gitbook.tw/chapters/github/fail-to-push.html"
      $ git push // if the last step succeeds

   // ***** self-check: Go to the branch on GitHub, and u shall see your local file has been uploaded

7. Sublime Text 3 Git Package
   a. Use package control to install "Git"
   // but do not know why cannot add and commit 




