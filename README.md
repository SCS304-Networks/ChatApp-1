# ChatApp-1

## Getting Started

This guide explains how to clone, work on the development branch, push, and pull changes for this repository.

### 1. Clone the Repository

Open your terminal and run:

```
git clone https://github.com/SCS304-Networks/ChatApp-1.git
```

This will create a folder named `ChatApp-1` with all the project files.

### 2. Change Directory

Navigate into the project folder:

```
cd ChatApp-1
```

### 3. Switch to the Development Branch

Before you start working, switch to the development branch:

```
git checkout development
```

If the branch does not exist locally, fetch it first:

```
git fetch origin
git checkout development
```

### 4. Make Your Changes

Edit, add, or remove files as needed for your work.

### 5. Stage and Commit Your Changes

1. Add your changes:
   ```
   git add .
   ```
2. Commit your changes:
   ```
   git commit -m "Your commit message"
   ```

### 6. Push to the Development Branch

Push your changes to the remote development branch:

```
git push origin development
```

If you are pushing for the first time, set the upstream:

```
git push --set-upstream origin development
```

### 7. Pull Latest Changes

To update your local repository with the latest changes from the remote development branch:

```
git pull origin development
```

---

## Building and Running the Application

### Requirements

- GCC compiler (e.g. [MinGW](https://www.mingw-w64.org/) on Windows or `gcc` on Linux/macOS)

### 1. Compile the Code

Navigate into the project folder and run:

```
gcc main.c auth.c chat.c utils.c -o chatapp
```

### 2. Run the Application

**On Windows:**

```
.\chatapp.exe
```

**On Linux/macOS:**

```
./chatapp
```

### 3. Merge Development into Main

```
git checkout main
git pull origin main
git merge development
```

If there are untracked files causing conflicts (e.g. `README.md`), stage them first:

```
git add README.md
git commit -m "Add README.md before merge"
git merge development
```

---

For more git commands and help, see [Git Documentation](https://git-scm.com/doc).
