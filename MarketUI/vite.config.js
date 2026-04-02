import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'
import { viteSingleFile } from 'vite-plugin-singlefile'

/* This defineConfig({}) block is used to define two plugins:
 * 
 * 1. react.
 * 2. viteSingleFile.
 *
 * React is, well, react.
 *
 * viteSingleFile allows us to build the frontend to a single file.
 * It inlines all assets into one index.html file.
 * This is required for launching the frontend with WebView.
 *
 * base: './' ensures we use *relative paths.*
 *
 * build {} is for build flags when running `npm run build`
 *    - minify : false, as in, be verbose and point to errors.
 *    - sourcemap: true, as in, map minified code back to source
 * https://vite.dev/config/
 */
export default defineConfig({
  plugins: [react(), viteSingleFile()],
  base: './',
  build: {
    minify: false,
    sourcemap: true,
  },
})
