/* vite.config.js
 *
 * This file houses the configuration information for Vite.
 * If any plugins are to be added or removed from the project,
 * you will be editing this file.
 */

import { defineConfig } from 'vite'                       // Vite build system
import react from '@vitejs/plugin-react'                  // Vite's React plugin.
import tailwindcss from '@tailwindcss/vite'               // Tailwindcss required for Shadcn/UI
import { viteSingleFile } from 'vite-plugin-singlefile'   // Builds the project to a single file for WebView.
import path from "path"                                   // Required for `path.resolve` in "@" alias.
import { fileURLToPath } from 'url'                       // Converts a file URL string into an absolute file path.

// Build const __dirname to somewhat match shadcn/ui docs.
const __dirname = path.dirname(fileURLToPath(import.meta.url));

/* This defineConfig({...}) block is used to define three plugins:
 * 
 * 1. react.
 * 2. tailwindcss
 * 3. viteSingleFile.
 *
 * React is, well, react.
 *
 * Tailwindcss is required for shadcn/ui. Tailwindcss self-describes
 * as: "A utility-first CSS framework packed with classes like flex,
 * pt-4, text-center and rotate-90 that can be composed to build any
 * design, directly in your markup."
 *
 * viteSingleFile allows us to build the frontend to a single file.
 * It inlines all assets into one index.html file.
 * This is required for launching the frontend with WebView.
 *
 * resolve...alias... "@" gives us an absolute path name alias for "@"
 *
 * base: './' ensures we use *relative paths.*
 *
 * build {} is for build flags when running `npm run build`
 *    - minify : false, as in, be verbose and point to errors.
 *    - sourcemap: true, as in, map minified code back to source
 * https://vite.dev/config/
 */
export default defineConfig({
  plugins: [
    react(),
    tailwindcss(),
    viteSingleFile()
  ],
  resolve: {
    alias: {
      "@": path.resolve(__dirname, "./src"),
    },
  },
  base: './',
  build: {
    minify: false,
    sourcemap: true,
  },
})
