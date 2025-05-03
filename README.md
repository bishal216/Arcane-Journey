
# SFML Docker Demo

This is a simple SFML project inside a Docker container.

## How to build and run:

1. **Build the Docker image:**

   ```bash
   docker build -t sfml-demo .
   ```
2. **Run the Docker container:**

   ```bash
   docker run -it --rm --env DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix sfml-demo
   ```

> This assumes you're running X11 on your local machine. If you're using a different environment, you may need to adjust the display settings.
