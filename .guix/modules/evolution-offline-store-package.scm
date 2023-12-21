;;; Commentary:
;;
;; GNU Guix development package.  To build and install, run:
;;
;;   guix package -f guix.scm
;;
;; To use as the basis for a development environment, run:
;;
;;   guix shell -D -f guix.scm
;;
;;; Code:

(define-module (evolution-offline-store-package)
  #:use-module (guix packages)
  #:use-module (guix gexp)
  #:use-module (guix git-download)
  #:use-module (guix build-system meson)
  #:use-module (guix build-system asdf)
  #:use-module (guix utils)
  #:use-module ((guix licenses) #:prefix license:)
  #:use-module (gnu packages gtk)
  #:use-module (gnu packages glib)
  #:use-module (gnu packages gettext)
  #:use-module (gnu packages cmake)
  #:use-module (gnu packages gnome)
  #:use-module (gnu packages autotools)
  #:use-module (gnu packages pkg-config)
  #:use-module (gnu packages build-tools)
  #:use-module (gnu packages freedesktop)
  #:use-module (gnu packages lisp-check)
  #:use-module (gnu packages lisp-check)
  #:use-module (gnu packages lisp-xyz)
  #:use-module (gnu packages texinfo)
  #:use-module (gnu packages xdisorg)
  #:use-module (gnu packages wm))

(define vcs-file?
  ;; Return true if the given file is under version control.
  (or (git-predicate (string-append (current-source-directory) "/../.."))
      (const #t)))                                ;not in a Git checkout

(define-public evolution-offline-store
  (package
    (name "evolution-offline-store")
    (version "3.24.4")
    (source (local-file "../.." "evolution-offline-store-checkout"
                        #:recursive? #t
                        #:select? vcs-file?))
    (build-system meson-build-system)
    (arguments
     (list #:configure-flags
           #~(list (string-append "-Dplugin-install-dir=" #$output "/lib/evolution/plugins"))
           #:validate-runpath? #f
           #:glib-or-gtk? #t))
    (native-search-paths
     (list
      (search-path-specification
       (variable "EVOLUTION_PLUGINDIR")
       (files (list "lib/evolution/plugins")))))
    (native-inputs
     (append
      (list
       gtk+
       `(,glib "bin")
       glib
       gnu-gettext
       cmake
       pkg-config
       evolution)
      (map cadr (package-inputs evolution))))
    (home-page "https://github.com/Tass0sm/evolution-offline-storage")
    (synopsis "Evolution plugin for offline storage of email")
    (description "Evolution plugin for offline storage of email")
    (license license:lgpl2.1)))

evolution-offline-store
