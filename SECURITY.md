# Security Policy

## Supported Versions

| Version | Supported          |
| ------- | ------------------ |
| 1.x.x   | :white_check_mark: |

## Reporting a Vulnerability

If you discover a security vulnerability within RediCraft, please send an e-mail to the maintainers via GitHub issues. All security vulnerabilities will be promptly addressed.

Please do not publicly disclose the vulnerability until it has been addressed by the team.

## Security Considerations

RediCraft is designed for use in trusted environments. The current version does not include:

- Authentication mechanisms
- Encryption for data in transit
- Access control mechanisms

These features are planned for future releases. For production environments, it is recommended to:

1. Run RediCraft in a secure, isolated network
2. Use firewall rules to restrict access
3. Monitor network traffic for suspicious activity