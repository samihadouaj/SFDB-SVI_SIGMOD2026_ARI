# Running Experiments on Chameleon Cloud (CHI@TACC)

Steps to reserve a bare metal machine, install Ubuntu 22.04 on it, connect, and set up Docker.

> **Two rules to remember**
> * A lease can last at most **7 days**.
> * A lease with **no running instance for 9 hours is terminated**. Launch the instance (step 4) as soon as the lease is ACTIVE.

---

## 1. Install the OpenStack client

Run on the machine you will connect from (e.g. your workstation).

```bash
python3 -m venv ~/chameleon-venv
source ~/chameleon-venv/bin/activate
pip install python-openstackclient chameleon-blazarclient
```

`python-openstackclient` manages machines; `chameleon-blazarclient` adds the `openstack reservation` commands.

## 2. Configure credentials

Credentials, key pairs, leases, instances and floating IPs are all **per site and per project**. Create the credential on the dashboard of the site you will use (here CHI@TACC). For another site (e.g. https://chi.uc.chameleoncloud.org), create a separate credential there, add it as a second entry in `clouds.yaml`, switch with `export OS_CLOUD=<name>`, and upload your key pair there too.

In the Chameleon GUI (https://chi.tacc.chameleoncloud.org), with your project selected:

1. **Identity > Application Credentials > Create Application Credential**
2. Check **Unrestricted** (required to create leases from the CLI).
3. Download the **clouds.yaml** file. The secret is shown only once.

Install it and select it:

```bash
mkdir -p ~/.config/openstack
mv clouds.yaml ~/.config/openstack/
chmod 600 ~/.config/openstack/clouds.yaml
export OS_CLOUD=openstack        # name under "clouds:" in the file; add to ~/.bashrc
```

Verify (should print a token and your project ID):

```bash
openstack token issue
```

Also upload your SSH public key once per project (create one with `ssh-keygen -t ed25519` if needed):

```bash
openstack keypair create --public-key ~/.ssh/id_ed25519.pub <key_name>
```

## 3. Reserve a machine (lease)

A lease reserves hardware for a time window. Dates are in **UTC**; omitting `--start-date` starts it now.

Make sure the node type you request has at least **750 GB of disk space, 500 GB of RAM and 24 physical cores**.

Reserve any node of a given type (e.g. `storage_hierarchy`: 512 GiB RAM, large disks):

```bash
openstack reservation lease create \
  --end-date "YYYY-MM-DD HH:MM" \
  --reservation resource_type=physical:host,min=1,max=1,resource_properties='["==","$node_type","storage_hierarchy"]' \
  <lease_name>
```

To get one specific node instead, use `'["==","$node_name","<node_name>"]'`. Node types and availability: Resource Discovery on chameleoncloud.org, or `openstack reservation host list`.

Check status (wait for **ACTIVE**):

```bash
openstack reservation lease show <lease_name> -c status
```

## 4. Install the operating system (launch an instance)

Launching an instance installs an OS image on the reserved node.

Get the **reservation ID** (the `"id"` inside `reservations`, not the lease ID):

```bash
openstack reservation lease show <lease_name> -c reservations
```

Launch with Ubuntu 22.04:

```bash
openstack server create \
  --flavor baremetal \
  --image CC-Ubuntu22.04 \
  --key-name <key_name> \
  --network sharednet1 \
  --hint reservation=<reservation_id> \
  <server_name>
```

Wait for **ACTIVE** (10 to 15 minutes; BUILD is normal):

```bash
openstack server show <server_name> -c status
```

If it shows ERROR: `openstack server show <server_name> -c fault`.

## 5. Attach a public IP (floating IP)

The node only has a private address. A floating IP makes it reachable from outside.

```bash
openstack floating ip create public                          # note floating_ip_address
openstack server add floating ip <server_name> <floating_ip>
```

## 6. Connect

```bash
ssh -i ~/.ssh/id_ed25519 cc@<floating_ip>
```

The user is always `cc` (passwordless sudo). If SSH reports **REMOTE HOST IDENTIFICATION HAS CHANGED** (the node was reinstalled), clear the old entry and retry:

```bash
ssh-keygen -R <floating_ip>
```

## 7. Verify the machine

Confirm you got what you need before installing anything:

```bash
lsb_release -a        # OS version: expect 22.04 (jammy)
free -g               # RAM (a 512 GiB node shows about 503)
lsblk -o NAME,SIZE,TYPE,ROTA,MODEL,FSTYPE,MOUNTPOINT   # disks; ROTA=0 means SSD/NVMe
df -h /               # free space on the root disk
```

Choose a directory with at least 750 GB free for the repository and the Docker data (called `<data_dir>` below). If `/` has enough space, any directory on it works. Otherwise, format and mount an unused large disk (e.g. at `/mnt/data`) and use that. If the node itself looks broken, open a ticket at the Chameleon Help Desk.

## 8. Install Docker

Optional, do first: store Docker data in `<data_dir>` (skip if `<data_dir>` is on `/`):

```bash
sudo mkdir -p /etc/docker <data_dir>/docker
echo '{"data-root": "<data_dir>/docker"}' | sudo tee /etc/docker/daemon.json
```

Add Docker's repository and install:

```bash
sudo apt update
sudo apt install -y ca-certificates curl
sudo install -m 0755 -d /etc/apt/keyrings
sudo curl -fsSL https://download.docker.com/linux/ubuntu/gpg -o /etc/apt/keyrings/docker.asc
sudo chmod a+r /etc/apt/keyrings/docker.asc
echo "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.asc] https://download.docker.com/linux/ubuntu $(. /etc/os-release && echo $VERSION_CODENAME) stable" | sudo tee /etc/apt/sources.list.d/docker.list
sudo apt update
sudo apt install -y docker-ce docker-ce-cli containerd.io docker-buildx-plugin docker-compose-plugin
```

Allow `cc` to run Docker without sudo:

```bash
sudo usermod -aG docker cc
newgrp docker
```

Verify:

```bash
docker run hello-world
docker info | grep "Docker Root Dir"
```

## 9. Run the experiments

Clone the repository into `<data_dir>` and follow the README from step 1.

---

**Before the lease ends:** copy results off the node. When the lease expires, the instance and all its data are deleted.
