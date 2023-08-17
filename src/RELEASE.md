OQS-BoringSSL snapshot 2023-06
==============================

About
-----

The **Open Quantum Safe (OQS) project** has the goal of developing and prototyping quantum-resistant cryptography.  More information on OQS can be found on our website: https://openquantumsafe.org/ and on Github at https://github.com/open-quantum-safe/.

**liboqs** is an open source C library for quantum-resistant cryptographic algorithms.

**open-quantum-safe/boringssl** is an integration of liboqs into (a fork of) BoringSSL.  The goal of this integration is to provide easy prototyping of quantum-resistant cryptography in TLS 1.3.  The integration should not be considered "production quality".

Release notes
=============

This is the 2023-06 snapshot release of OQS-BoringSSL, released on June 26, 2023. This release is intended to be used with liboqs version 0.8.0.

What's New
----------

This is the seventh snapshot release of OQS-BoringSSL.  It is based on BoringSSL commit [ae88f198a49d77993e9c44b017d0e69c810dc668](https://github.com/google/boringssl/commit/ae88f198a49d77993e9c44b017d0e69c810dc668).

- Upstream update
- Update of algorithms in line with [liboqs v0.8.0](https://github.com/open-quantum-safe/liboqs/releases/tag/0.8.0)

Previous release notes
----------------------

- [OQS-BoringSSL snapshot 2022-08](https://github.com/open-quantum-safe/boringssl/releases/tag/OQS-BoringSSL-snapshot-2022-08) aligned with liboqs 0.7.2 (August 24, 2022)
- [OQS-BoringSSL snapshot 2021-08](https://github.com/open-quantum-safe/boringssl/releases/tag/OQS-BoringSSL-snapshot-2021-08) aligned with liboqs 0.7.0 (August 11, 2021)
- [OQS-BoringSSL snapshot 2021-03](https://github.com/open-quantum-safe/boringssl/releases/tag/OQS-BoringSSL-snapshot-2021-03) aligned with liboqs 0.5.0 (March 26, 2021)
- [OQS-BoringSSL snapshot 2020-08](https://github.com/open-quantum-safe/boringssl/releases/tag/OQS-BoringSSL-snapshot-2020-08) aligned with liboqs 0.4.0 (August 11, 2020)
- [OQS-BoringSSL snapshot 2020-07](https://github.com/open-quantum-safe/boringssl/releases/tag/OQS-BoringSSL-snapshot-2020-07) aligned with liboqs 0.3.0 (July 10, 2020)

## What's Changed
* removing Picnic,NTRUprime,Rainbow,Saber by @baentsch in https://github.com/open-quantum-safe/boringssl/pull/88
* Reverted back SSL_MAX_CERT_LIST_DEFAULT value since large algs are gone by @christianpaquin in https://github.com/open-quantum-safe/boringssl/pull/89
* Re-ran objects.go script after OQS alg changes by @christianpaquin in https://github.com/open-quantum-safe/boringssl/pull/90
* Update to upstream 5511fa8 by @christianpaquin in https://github.com/open-quantum-safe/boringssl/pull/91
* remove NTRU by @baentsch in https://github.com/open-quantum-safe/boringssl/pull/95
* Fix expired test cert failure by @christianpaquin in https://github.com/open-quantum-safe/boringssl/pull/94
* Syncing algorithms with liboqs. by @xvzcf in https://github.com/open-quantum-safe/boringssl/pull/99
* Update to upstream ae88f19 by @Raytonne in https://github.com/open-quantum-safe/boringssl/pull/100

## New Contributors
* @Raytonne made their first contribution in https://github.com/open-quantum-safe/boringssl/pull/100

**Full Changelog**: https://github.com/open-quantum-safe/boringssl/compare/OQS-BoringSSL-snapshot-2022-08...OQS-BoringSSL-snapshot-2023-06

