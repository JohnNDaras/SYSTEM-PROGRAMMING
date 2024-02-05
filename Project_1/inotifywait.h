#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>
#include <errno.h>
#include <sys/signalfd.h>
#include <linux/inotify.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/wait.h>



/* Structure to keep track of monitored directories */
typedef struct {
  /* Path of the directory */
  char *path;
  /* inotify watch descriptor */
  int wd;
} monitored_t;

/* Size of buffer to use when reading inotify events */
#define INOTIFY_BUFFER_SIZE 8192

/* Enumerate list of FDs to poll */
enum {
  FD_POLL_SIGNAL = 0,
  FD_POLL_INOTIFY,
  FD_POLL_MAX
};

char pipebuf[45];
char backslash[1] = "/";

/* FANotify-like helpers to iterate events */
#define IN_EVENT_DATA_LEN (sizeof(struct inotify_event))
#define IN_EVENT_NEXT(event, length)            \
  ((length) -= (event)->len,                    \
   (struct inotify_event*)(((char *)(event)) +	\
                           (event)->len))
#define IN_EVENT_OK(event, length)                  \
  ((long)(length) >= (long)IN_EVENT_DATA_LEN &&	    \
   (long)(event)->len >= (long)IN_EVENT_DATA_LEN && \
   (long)(event)->len <= (long)(length))

/* Setup inotify notifications (IN) mask. All these defined in inotify.h. */
static int event_mask =
  (IN_ACCESS |        /* File accessed */
   IN_ATTRIB |        /* File attributes changed */
   IN_OPEN   |        /* File was opened */
   IN_CLOSE_WRITE |   /* Writtable File closed */
   IN_CLOSE_NOWRITE | /* Unwrittable File closed */
   IN_CREATE |        /* File created in directory */
   IN_DELETE |        /* File deleted in directory */
   IN_DELETE_SELF |   /* Directory deleted */
   IN_MODIFY |        /* File modified */
   IN_MOVE_SELF |     /* Directory moved */
   IN_MOVED_FROM |    /* File moved away from the directory */
   IN_MOVED_TO);      /* File moved into the directory */

/* Array of directories being monitored */
static monitored_t *monitors;
static int n_monitors;

static void
__event_process (struct inotify_event *event)
{
  int i;

  /* Need to loop all registered monitors to find the one corresponding to the
   * watch descriptor in the event. A hash table here would be quite a better
   * approach. */
  for (i = 0; i < n_monitors; ++i)
    {
      /* If watch descriptors match, we found our directory */
      if (monitors[i].wd == event->wd)
        {
          if (event->len > 0)
            printf ("Received event in '%s/%s': ",
                    monitors[i].path,
                    event->name);
          else
            printf ("Received event in '%s': ",
                    monitors[i].path);

          if (event->mask & IN_ACCESS)
            printf ("\tIN_ACCESS\n");
          if (event->mask & IN_ATTRIB)
            printf ("\tIN_ATTRIB\n");
          if (event->mask & IN_OPEN)
            printf ("\tIN_OPEN\n");
          if (event->mask & IN_CLOSE_WRITE)
            printf ("\tIN_CLOSE_WRITE\n");
          if (event->mask & IN_CLOSE_NOWRITE)
            printf ("\tIN_CLOSE_NOWRITE\n");
          if (event->mask & IN_CREATE)
            printf ("\tIN_CREATE\n");
          if (event->mask & IN_DELETE)
            printf ("\tIN_DELETE\n");
          if (event->mask & IN_DELETE_SELF)
            printf ("\tIN_DELETE_SELF\n");
          if (event->mask & IN_MODIFY)
            printf ("\tIN_MODIFY\n");
          if (event->mask & IN_MOVE_SELF)
            printf ("\tIN_MOVE_SELF\n");
          if (event->mask & IN_MOVED_FROM)
            printf ("\tIN_MOVED_FROM (cookie: %d)\n",
                    event->cookie);
          if (event->mask & IN_MOVED_TO)
            printf ("\tIN_MOVED_TO (cookie: %d)\n",
                    event->cookie);
			
		  sprintf(pipebuf, "%s%s%s", monitors[i].path, backslash,event->name);		  
		  
          fflush (stdout);
          return;
        }
    }
}

static void __shutdown_inotify (int inotify_fd)
{
  int i;

  for (i = 0; i < n_monitors; ++i)
    {
      free (monitors[i].path);
      inotify_rm_watch (inotify_fd, monitors[i].wd);
    }
  free (monitors);
  close (inotify_fd);
}

static int __initialize_inotify (int          argc,
                      const char **argv)
{
  int i;
  int inotify_fd;

  /* Create new inotify device */
  if ((inotify_fd = inotify_init ()) < 0)
    {
      fprintf (stderr,
               "Couldn't setup new inotify device: '%s'\n",
               strerror (errno));
      return -1;
    }

  /* Allocate array of monitor setups */
  n_monitors = argc - 1;
  monitors = malloc (n_monitors * sizeof (monitored_t));

  /* Loop all input directories, setting up watches */
  for (i = 0; i < n_monitors; ++i)
    {
      monitors[i].path = strdup (argv[i + 1]);
      if ((monitors[i].wd = inotify_add_watch (inotify_fd,
                                               monitors[i].path,
                                               event_mask)) < 0)
        {
          fprintf (stderr,
                   "Couldn't add monitor in directory '%s': '%s'\n",
                   monitors[i].path,
                   strerror (errno));
          exit (EXIT_FAILURE);
        }
      printf ("Started monitoring directory '%s'...\n",
              monitors[i].path);
    }

  return inotify_fd;
}

static void __shutdown_signals (int signal_fd)
{
  close (signal_fd);
}

static int __initialize_signals (void)
{
  int signal_fd;
  sigset_t sigmask;

  /* We want to handle SIGINT and SIGTERM in the signal_fd, so we block them. */
  sigemptyset (&sigmask);
  sigaddset (&sigmask, SIGINT);
  sigaddset (&sigmask, SIGTERM);

  if (sigprocmask (SIG_BLOCK, &sigmask, NULL) < 0)
    {
      fprintf (stderr,
               "Couldn't block signals: '%s'\n",
               strerror (errno));
      return -1;
    }

  /* Get new FD to read signals from it */
  if ((signal_fd = signalfd (-1, &sigmask, 0)) < 0)
    {
      fprintf (stderr,
               "Couldn't setup signal FD: '%s'\n",
               strerror (errno));
      return -1;
    }

  return signal_fd;
}
