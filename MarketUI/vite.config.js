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
 * https://vite.dev/config/
 */
export default defineConfig({
  plugins: [react(), viteSingleFile()],
  base: './',
})
